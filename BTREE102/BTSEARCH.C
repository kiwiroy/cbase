/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btsearch.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* library headers */
#include <blkio.h>

/* local headers */
#include "btree_.h"

/*man---------------------------------------------------------------------------
NAME
     btsearch - search a btree

SYNOPSIS
     #include <btree.h>

     int btsearch(btp, buf)
     btree_t *btp;
     const void *buf;

DESCRIPTION
     The btsearch function searches the btree btp for the key pointed
     to by buf.  If it is found, the cursor is set to the location of
     the key and 1 is returned.  If it is not found, the cursor is set
     to the next higher key and 0 is returned.

     btsearch will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       buf is the NULL pointer.
     [BTELOCK]      btp is not read locked.

SEE ALSO
     btdelcur, btdelete, btinsert.

DIAGNOSTICS
     Upon successful completion, a value of 1 is returned if the key
     was found or a value of 0 if it was not found.  On failure, a
     value of -1 is returned, and errno set to indicate the error.
     
------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btsearch(btree_t *btp, const void *buf)
#else
int btsearch(btp, buf)
btree_t *btp;
const void *buf;
#endif
{
	int found = 0;		/* key found flag */

	/* validate arguments */
	if (!bt_valid(btp) || buf == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* check locks */
	if (!(btp->flags & BTRDLCK)) {
		errno = BTELOCK;
		return -1;
	}

	/* search to position to insert */
	found = bt_search(btp, buf);
	if (found == -1) {
		BTEPRINT;
		return -1;
	}

	/* check if cursor on empty slot */
	if (found == 0) {
		if (btp->cbtpos.key > btp->cbtnp->n) {
			btp->cbtpos.key = btp->cbtnp->n;
			if (btnext(btp) == -1) {
				BTEPRINT;
				return -1;
			}
		}
	}

	return found == 1 ? 1: 0;
}
