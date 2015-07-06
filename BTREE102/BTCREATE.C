/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btcreate.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <blkio.h>

/* local headers */
#include "btree_.h"

/*man---------------------------------------------------------------------------
NAME
     btcreate - create a btree

SYNOPSIS
     int btcreate(filename, m, keysize, fldc, fldv)
     const char *filename;
     int m;
     size_t keysize;
     int fldc;
     const btfield_t fldv[];

DESCRIPTION
     The btcreate function creates the btree file named by filename.
     m is the degree of the btree and keysize is the size of the keys.

     fldc is the field count.  It specifies the number of fields in
     the keys stored in this btree.  fldv is an array of field
     definition structures.  fldv must have fldc elements.  The field
     definition structure is defined in <btree.h> as type btfield_t.
     It has the following members.

          size_t offset;      /* offset of field in key *\/
          size_t len;         /* field length *\/
          int (*cmp)(void *p1, void *p2, size_t n);
                              /* comparison function for field *\/
          int flags;          /* flags *\/

     offset and len specify the location and length of the field,
     respectively.  cmp is a pointer to the user written comparison
     function which defines the sort sequence for the field; p1 and p2
     point to the two fields to be compared and n is the field length.
     The return value must be less than, equal to, or greater than
     zero if p1 is less than, equal to, or greater than p2,
     respectively.  btree field flags values are constructed by
     bitwise OR-ing together flags from the following list (one and
     only one of the first two may be used).

     BT_FASC        ascending order
     BT_FDSC        descending order

     The major sort is on the first field in fldv.  For keys for which
     the first fields are identical (as reported by the comparison
     function for that field), a minor sort is performed using the
     second field, etc.  The physical order of the fields in the key is
     not significant, and fields may overlap.

     btopen will fail if one or more of the following is true:

     [EEXIST]       The named btree file exists.
     [EINVAL]       filename is the NULL pointer.
     [EINVAL]       m is less than 3.
     [EINVAL]       keysize is less than 1.
     [EINVAL]       fldc is less than 1.
     [EINVAL]       fldv is the NULL pointer.
     [EINVAL]       fldv contains an invalid field definition.
     [BTEMFILE]     Too many open btrees.  The maximum is defined as
                    BTOPEN_MAX in btree.h.

SEE ALSO
     btopen.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btcreate(const char *filename, int m, size_t keysize, int fldc, const btfield_t fldv[])
#else
int btcreate(filename, m, keysize, fldc, fldv)
const char *filename;
int m;
size_t keysize;
int fldc;
const btfield_t fldv[];
#endif
{
	btree_t *	btp	= NULL;
	int		terrno	= 0;		/* tmp errno */

	/* validate arguments */
	if (filename == NULL || m < 3 || !bt_fvalid(keysize, fldc, fldv)) {
		errno = EINVAL;
		return -1;
	}

	/* find free slot in btb table */
	for (btp = btb; btp < (btb + BTOPEN_MAX); ++btp) {
		if (!(btp->flags & BTOPEN)) {
			break;		/* found */
		}
	}
	if (btp >= btb + BTOPEN_MAX) {
		errno = BTEMFILE;	/* no free slots */
		return -1;
	}

	/* load btree_t structure */
	btp->bthdr.flh = NIL;
	btp->bthdr.m = m;
	btp->bthdr.keysize = keysize;
	btp->bthdr.flags = 0;
	btp->bthdr.root = NIL;
	btp->bthdr.first = NIL;
	btp->bthdr.last = NIL;
	btp->bthdr.keycnt = 0;
	btp->bthdr.height = 0;
	btp->bp = NULL;
	btp->flags = BTREAD | BTWRITE;
	btp->fldc = 0;				/* fields */
	btp->fldv = NULL;
	btp->cbtpos.node = NIL;			/* cursor */
	btp->cbtpos.key = 0;
	btp->cbtnp = NULL;
	btp->sp = NULL;

	/* create file */
	btp->bp = bopen(filename, "c", sizeof(bthdr_t), (size_t)1, (size_t)0);
	if (btp->bp == NULL) {
		if (errno != EEXIST) BTEPRINT;
		terrno = errno;
		memset(btp, 0, sizeof(*btb));
		btp->flags = 0;
		errno = terrno;
		return -1;
	}

	/* write header to file */
	if (bputh(btp->bp, &btp->bthdr) == -1) {	/* header */
		BTEPRINT;
		terrno = errno;
		bclose(btp->bp);
		memset(btp, 0, sizeof(*btb));
		btp->flags = 0;
		errno = terrno;
		return -1;
	}

	/* close btp */
	if (btclose(btp) == -1) {
		BTEPRINT;
		return -1;
	}

	return 0;
}
