/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btsetcur.c	1.5 - 91/09/23" */

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
     btsetcur - set btree cursor

SYNOPSIS
     #include <btree.h>

     int btsetcur(btp, btposp)
     btree_t *btp;
     const btpos_t *btposp;

DESCRIPTION
     The btsetcur function sets the current cursor position for btree
     btp to the value in btposp.  btposp should point to a cursor
     value saved previously with btgetcur.  If btposp is the NULL
     pointer, the cursor is set to null.  It is important to remember
     that a btree position is not valid after an insertion, deletion,
     or unlock.

     btsetcur will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [BTELOCK]      btp is not locked.
     [BTENKEY]      btposp points to an invalid cursor value.
     [BTENOPEN]     btp is not open.

SEE ALSO
     btgetcur.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btsetcur(btree_t *btp, const btpos_t *btposp)
#else
int btsetcur(btp, btposp)
btree_t *btp;
const btpos_t *btposp;
#endif
{
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

	/* set cursor */
	if (btposp == NULL) {
		btp->cbtpos.node = NIL;		/* set cursor to null */
		btp->cbtpos.key = 0;
	} else {
		memcpy(&btp->cbtpos, btposp, sizeof(btp->cbtpos));
	}

	/* read in new current node */
	if (btp->cbtpos.node == NIL) {
		bt_ndinit(btp, btp->cbtnp);
	} else {
		if (bt_ndget(btp, btp->cbtpos.node, btp->cbtnp) == -1) {
			BTEPRINT;
			if (errno == BEEOF) errno = BTENKEY;
			return -1;
		}
		if (btp->cbtnp->n == 0) {	/* check if block empty */
			btp->cbtpos.node = NIL;
			btp->cbtpos.key = 0;
			bt_ndinit(btp, btp->cbtnp);
			errno = BTENKEY;
			return -1;
		}
	}

	/* check if key number in range */
	if (btp->cbtpos.key > btp->cbtnp->n) {
		btp->cbtpos.node = NIL;
		btp->cbtpos.key = 0;
		bt_ndinit(btp, btp->cbtnp);
		errno = BTENKEY;
		return -1;
	}

	return 0;
}
