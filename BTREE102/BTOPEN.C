/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btopen.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#ifdef AC_STDLIB
#include <stdlib.h>
#endif
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <blkio.h>

/* local headers */
#include "btree_.h"

/* btree control structure table definition */
btree_t btb[BTOPEN_MAX];

/*man---------------------------------------------------------------------------
NAME
     btopen - open a btree

SYNOPSIS
     btree_t *btopen(filename, type, fldc, fldv)
     const char *filename;
     const char *type;
     int fldc;
     const btfield_t fldv[];

DESCRIPTION
     The btopen function opens the file named by filename as a btree.
     A pointer to the btree_t control structure associated with the
     file is returned.

     type is a character string having one of the following values:

          "r"            open for reading
          "r+"           open for update (reading and writing)

     See btcreate for explanation of the field count fldc and the
     field definition list fldv.

     btopen will fail if one or more of the following is true:

     [EINVAL]       filename is the NULL pointer.
     [EINVAL]       type is not "r" or "r+".
     [EINVAL]       fldc is less than 1.
     [EINVAL]       fldv is the NULL pointer.
     [EINVAL]       fldv contains an invalid field definition.
     [ENOENT]       The named btree file does not exist.
     [BTEMFILE]     Too many open btrees.  The maximum is defined as
                    BTOPEN_MAX in <btree.h>.

SEE ALSO
     btclose, btcreate.

DIAGNOSTICS
     On failure btopen returns a NULL pointer, and errno is set to
     indicate the error.

NOTES
     The same field count and field definition list with which the
     btree was created must be used each time the btree is opened.
     Otherwise the results are undefined.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
btree_t *btopen(const char *filename, const char *type, int fldc, const btfield_t fldv[])
#else
btree_t *btopen(filename, type, fldc, fldv)
const char *filename;
const char *type;
int fldc;
const btfield_t fldv[];
#endif
{
	btree_t *	btp	= NULL;
	int		terrno	= 0;		/* tmp errno */

	/* validate arguments */
	if (filename == NULL || type == NULL) {
		errno = EINVAL;
		return NULL;
	}

	/* find free slot in btb table */
	for (btp = btb; btp < btb + BTOPEN_MAX; ++btp) {
		if (!(btp->flags & BTOPEN)) {
			break;		/* found */
		}
	}
	if (btp >= btb + BTOPEN_MAX) {
		errno = BTEMFILE;	/* no free slots */
		return NULL;
	}

	/* open file */
	if (strcmp(type, BT_READ) == 0) {
		btp->flags = BTREAD;
	} else if (strcmp(type, BT_RDWR) == 0) {
		btp->flags = BTREAD | BTWRITE;
	} else {
		errno = EINVAL;
		return NULL;
	}
	btp->bp = bopen(filename, type, sizeof(bthdr_t), (size_t)1, (size_t)0);
	if (btp->bp == NULL) {
		if ((errno != ENOENT) && (errno != EACCES)) BTEPRINT;
		terrno = errno;
		memset(btp, 0, sizeof(*btb));
		btp->flags = 0;
		errno = terrno;
		return NULL;
	}

	/* load btree_t structure */
	memset(&btp->bthdr, 0, sizeof(btp->bthdr));	/* header */
	if (!bt_fvalid(UINT_MAX, fldc, fldv)) {
		bclose(btp->bp);
		memset(btp, 0, sizeof(*btb));
		btp->flags = 0;
		errno = EINVAL;
		return NULL;
	}
	btp->fldc = fldc;		/* field count */
	btp->fldv = NULL;
	btp->cbtpos.node = NIL;		/* cursor */
	btp->cbtpos.key = 0;
	btp->cbtnp = NULL;
	btp->sp = NULL;

	/* copy field definition array */
	btp->fldv = (btfield_t *)calloc((size_t)btp->fldc, sizeof(*btp->fldv));
	if (btp->fldv == NULL) {
		BTEPRINT;
		terrno = errno;
		bclose(btp->bp);
		memset(btp, 0, sizeof(*btp));
		btp->flags = 0;
		errno = terrno;
		return NULL;
	}
	memcpy(btp->fldv, fldv, btp->fldc * sizeof(*btp->fldv));

	return btp;
}
