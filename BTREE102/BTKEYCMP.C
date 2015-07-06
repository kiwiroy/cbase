/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btkeycmp.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* library headers */
#include <blkio.h>

/* local headers */
#include "btree_.h"

/*man---------------------------------------------------------------------------
NAME
     btkeycmp - compare btree keys

SYNOPSIS
     #include <btree.h>

     int btkeycmp(btp, buf1, buf2)
     btree_t *btp;
     const void *buf1;
     const void *buf2;

DESCRIPTION
     The btkeycmp function compares the keys pointed to be buf1 and
     buf2.  buf1 and buf2 must point to keys of the size stored in
     btree btp.  The value returned will be less than, equal to, or
     greater than 0, according as the key pointed to by buf1 is less
     than, equal to, or greater than the key pointed to by buf2.

     btkeycmp will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       buf is the NULL pointer.

SEE ALSO
     btsearch.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btkeycmp(btree_t *btp, const void *buf1, const void *buf2)
#else
int btkeycmp(btp, buf1, buf2)
btree_t *btp;
const void *buf1;
const void *buf2;
#endif
{
	int	cmp	= 0;		/* result of key comparison */
	int	fld	= 0;		/* field number */

	/* compare each field */
	for (fld = 0; fld < btp->fldc; ++fld) {
		cmp = (*btp->fldv[fld].cmp)((char *)buf1 + btp->fldv[fld].offset,
			(char *)buf2 + btp->fldv[fld].offset, btp->fldv[fld].len);
		if (cmp != 0) {
			if (btp->fldv[fld].flags & BT_FDSC) {
				cmp = -cmp;
			}
			break;
		}
	}

	return cmp;
}
