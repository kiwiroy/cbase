/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btnext.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* library headers */
#include <blkio.h>

/* local headers */
#include "btree_.h"

/*man---------------------------------------------------------------------------
NAME
     btnext - next btree key

SYNOPSIS
     #include <btree.h>

     int btnext(btp)
     btree_t *btp;

DESCRIPTION
     The btnext function advances the cursor of btree btp to the next
     key.  If cursor is currently null, it will be advanced to the
     first key.  If the cursor is currently on the last key, it will
     advance to null.  If the tree is empty, the cursor will remain on
     null.

     btnext will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [BTELOCK]      btp is not locked.
     [BTENOPEN]     btp is not open.

SEE ALSO
     btcursor, btfirst, btlast, btprev.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btnext(btree_t *btp)
#else
int btnext(btp)
btree_t *btp;
#endif
{
	int terrno = 0;		/* tmp errno */

	/* validate arguments */
	if (!bt_valid(btp)) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(btp->flags & BTOPEN)) {
		errno = BTENOPEN;
		return -1;
	}

	/* check locks */
	if (!(btp->flags & BTLOCKS)) {
		errno = BTELOCK;
		return -1;
	}

	/* advance cursor to next key in current node */
	if (btp->cbtpos.node != NIL) {
		if (++btp->cbtpos.key <= btp->cbtnp->n) {
			return 0;
		}
	}

	/* advance cursor to null */
	if (btp->cbtpos.node == btp->bthdr.last) {
		btp->cbtpos.node = NIL;
		btp->cbtpos.key = 0;
		bt_ndinit(btp, btp->cbtnp);
		return 0;
	}

	/* advance cursor to first key in next node */
	if (btp->cbtpos.node == NIL) {
		btp->cbtpos.node = btp->bthdr.first;
	} else {
		btp->cbtpos.node = btp->cbtnp->rsib;
	}
	if (bt_ndget(btp, btp->cbtpos.node, btp->cbtnp) == -1) {
		BTEPRINT;
		terrno = errno;
		btp->cbtpos.node = NIL;
		btp->cbtpos.key = 0;
		bt_ndinit(btp, btp->cbtnp);
		errno = terrno;
		return -1;
	}
	btp->cbtpos.key = 1;

	return 0;
}
