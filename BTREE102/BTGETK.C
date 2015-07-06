/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btgetk.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <blkio.h>

/* local headers */
#include "btree_.h"

/*man---------------------------------------------------------------------------
NAME
     btgetk - read current btree key

SYNOPSIS
     #include <btree.h>

     int btgetk(btp, buf)
     btree_t *btp;
     void *buf;

DESCRIPTION
     The btgetk function reads the key from the current cursor
     position in btree btp into buf.  buf must point to a storage area
     as large as the key size for btp.

     btgetk will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       buf is the NULL pointer.
     [BTELOCK]      btp is not read locked.
     [BTENKEY]      The cursor is null.
     [BTENOPEN]     btp is not open.

SEE ALSO
     btcursor, btkeysize, btsearch.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btgetk(btree_t *btp, void *buf)
#else
int btgetk(btp, buf)
btree_t *btp;
void *buf;
#endif
{
	/* validate arguments */
	if (!bt_valid(btp) || buf == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(btp->flags & BTOPEN)) {
		errno = EINVAL;
		return -1;
	}

	/* check locks */
	if (!(btp->flags & BTRDLCK)) {
		errno = BTELOCK;
		return -1;
	}

	/* initialize return value */
	memset(buf, 0, btp->bthdr.keysize);

	/* read key value */
	if (btp->cbtpos.node == NIL) {
		errno = BTENKEY;
		return -1;
	}
	memcpy(buf, bt_kykeyp(btp, btp->cbtnp, btp->cbtpos.key), btp->bthdr.keysize);

	return 0;
}
