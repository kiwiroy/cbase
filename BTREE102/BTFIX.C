/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btfix.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#include <stdio.h>
#ifdef AC_STDLIB
#include <stdlib.h>
#endif
#ifdef AC_STRING
#include <string.h>
#endif

/*library headers */
#include <blkio.h>

/* local headers */
#include "btree_.h"

#undef min
#define min(a,b)	((a) < (b) ? (a) : (b))

/*man---------------------------------------------------------------------------
NAME
     btfix - fix a corrupt btree file

SYNOPSIS
     int btfix(filename, m, keysize, fldc, fldv)
     const char *filename;
     int m;
     size_t keysize;
     int fldc;
     const btfield_t fldv[];

DESCRIPTION
     The btfix function reconstructs a corrupt btree.  The parameters
     are the same as for btcreate.  It may be specified that the
     degree of the btree and the key size be read from the header by
     passing a value of zero for m and keysize, respectively.  These
     parameters are provided for cases where the header is corrupt.

     btfix will fail if one or more of the following is true:

     [EINVAL]       filename is the NULL pointer.
     [EINVAL]       fldc is less than 1.
     [EINVAL]       fldv is the NULL pointer.
     [EINVAL]       fldv contains an invalid field
                    definition.
     [ENOENT]       The named btree file does not exist.
     [BTEEOF]       Incomplete file header.
     [BTEMFILE]     Too many open btrees.  The maximum
                    is defined as BTOPEN_MAX in btree.h.

SEE ALSO
     btcreate.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

NOTES
     It is recommended that a backup copy of the corrupted file be
     made before calling a reconstruction routine such as btfix.

     If the btree contains only redundant data (i.e., an index), it is
     better to rebuild it from scratch rather than attempt to fix it.

     btfix uses a temporary file to reconstruct the btree.  If
     security is important, a call should be added to set the
     permissions of the temporary file (e.g., chmod in UNIX) before
     data is written to it.  This has not been included because such a
     call is not portable.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btfix(const char *filename, int m, size_t keysize, int fldc, const btfield_t fldv[])
#else
int btfix(filename, m, keysize, fldc, fldv)
const char *filename;
int m;
size_t keysize;
int fldc;
const btfield_t fldv[];
#endif
{
	int		terrno	= 0;		/* tmp errno */
	bthdr_t		bthdr;			/* btree header */
	char		tmpbtname[L_tmpnam + 1];/* temporary btree file name */
	btree_t *	btp	= NULL;		/* btree */
	btree_t		btree;			/* used for bt_blksize */
	BLKFILE *	bp	= NULL;		/* block file */
	btnode_t *	btnp	= NULL;		/* btree node */
	bpos_t		bn	= 0;		/* block number */
	int		k	= 0;		/* counter */
	void *		buf	= NULL;

	/* validate arguments */
	if (filename == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* open original btree file */
	bp = bopen(filename, "r", sizeof(bthdr_t), (size_t)1, (size_t)0);
	if (bp == NULL) {
		if (errno != ENOENT) BTEPRINT;
		return -1;
	}
	if (lockb(bp, B_RDLCK, (bpos_t)0, (bpos_t)0) == -1) {	/* read lock */
		terrno = errno;
		bclose(bp);
		errno = terrno;
		return -1;
	}

	/* read header and set block size */
	if (bgeth(bp, &bthdr) == -1) {
		BTEPRINT;
		if (errno == BEEOF) errno = BTEEOF;
		terrno = errno;
		bclose(bp);
		errno = terrno;
		return -1;
	}
	if (m == 0) {
		m = bthdr.m;
	}
	if (keysize == 0) {
		keysize = bthdr.keysize;
	}
	btree.bthdr.m = m;		/* set block size */
	btree.bthdr.keysize = keysize;
	if (bsetvbuf(bp, NULL, bt_blksize(&btree), (size_t)0) == -1) {
		BTEPRINT;
		terrno = errno;
		bclose(bp);
		errno = terrno;
		return -1;
	}

	/* create temporary btree */
	tmpnam(tmpbtname);
	if (btcreate(tmpbtname, m, keysize, fldc, fldv) == -1){
		BTEPRINT;
		terrno = errno;
		bclose(bp);
		errno = terrno;
		return -1;
	}
	/*
	If security is important, insert a call here
	to set the permissions of file tmpbtname.  In UNIX, for
	example,
	#include <sys/types.h>
	#include <sys/stat.h>
	if (chmod(tmpbtname, S_IRUSR | S_IWUSR) == -1) {
		BTEPRINT;
		terrno = errno;
		bclose(bp);
		errno = terrno;
		return -1;
	}
	*/
	btp = btopen(tmpbtname, "r+", fldc, fldv);
	if (btp == NULL) {
		BTEPRINT;
		terrno = errno;
		bclose(bp);
		errno = terrno;
		return -1;
	}

	/* create in-core btree node */
	btnp = bt_ndalloc(btp);
	if (btnp == NULL) {
		BTEPRINT;
		terrno = errno;
		bclose(bp);
		btclose(btp);
		errno = terrno;
		return -1;
	}

	/* create buffer for reading blocks */
	buf = calloc((size_t)1, bt_blksize(&btree));
	if (buf == NULL) {
		BTEPRINT;
		terrno = errno;
		bclose(bp);
		errno = terrno;
		return -1;
	}

	/* write lock temporary btree */
	if (btlock(btp, BT_WRLCK) == -1) {
		terrno = errno;
		bclose(bp);
		free(buf);
		btclose(btp);
		bt_ndfree(btnp);
		errno = terrno;
		return -1;
	}

	/* main loop */
	for (bn = 1; ; ++bn) {
		/* get next node */
		if (bgetb(bp, bn, buf) == -1) {
			if (errno == BEEOF) {
				break;
			}
			BTEPRINT;
			terrno = errno;
			bclose(bp);
			free(buf);
			btclose(btp);
			bt_ndfree(btnp);
			errno = terrno;
			return -1;
		}

		/* convert file node to in-core format */
		memcpy(btnp, buf, offsetof(btnode_t, keyv));
		memcpy(btnp->keyv,
			((char *)buf + offsetof(btnode_t, keyv)),
			((btp->bthdr.m - 1) * btp->bthdr.keysize));
		memcpy(btnp->childv,
			((char *)buf + offsetof(btnode_t, keyv) +
				((btp->bthdr.m - 1) * btp->bthdr.keysize)),
			(btp->bthdr.m * sizeof(*btnp->childv)));

		/* extract each key from node and insert in new btree */
		if ((btnp->n != 0) && bt_ndleaf(btnp)) {
			for (k = 1; k <= min(btnp->n, bt_ndmax(btp)); ++k) {
				if (btinsert(btp, bt_kykeyp(btp, btnp, k)) == -1) {
					if (errno != BTEDUP) {
						BTEPRINT;
						terrno = errno;
						bclose(bp);
						free(buf);
						btclose(btp);
						bt_ndfree(btnp);
						errno = terrno;
						return -1;
					}
				}
			}
		}
	}

	/* free memory */
	free(buf);
	bt_ndfree(btnp);

	/* close and remove original btree file */
	if (bclose(bp) == -1) {
		BTEPRINT;
		terrno = errno;
		btclose(btp);
		errno = terrno;
		return -1;
	}
	if (remove(filename) != 0) {
		BTEPRINT;
		terrno = errno;
		btclose(btp);
		errno = terrno;
		return -1;
	}

	/* move temporary file to filename */
	if (rename(tmpbtname, filename) != 0) {
		BTEPRINT;
		terrno = errno;
		btclose(btp);
		errno = terrno;
		return -1;
	}

	/* close and remove temporary file */
	if (btclose(btp) == -1) {
		BTEPRINT;
		return -1;
	}
	if (remove(tmpbtname) != 0) {
		BTEPRINT;
		return -1;
	}

	return 0;
}
