/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btfirst.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* library headers */
#include <blkio.h>

/* local headers */
#include "btree_.h"

/*man---------------------------------------------------------------------------
NAME
     btfirst - first btree key

SYNOPSIS
     #include <btree.h>

     int btfirst(btp)
     btree_t *btp;

DESCRIPTION
     The btfirst function positions the cursor of btree btp on the
     first key in that btree.

     btfirst will fail if one or more of the following is true.

     [EINVAL]       btp is not a valid btree pointer.
     [BTELOCK]      btp is not locked.
     [BTENKEY]      btp is empty.
     [BTENOPEN]     btp is not open.

SEE ALSO
     btkeycnt, btlast, btnext, btprev.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btfirst(btree_t *btp)
#else
int btfirst(btp)
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

	/* set cursor to first key */
	btp->cbtpos.node = btp->bthdr.first;
	btp->cbtpos.key = 1;

	/* check if tree is empty */
	if (btp->cbtpos.node == NIL) {
		btp->cbtpos.key = 0;
		bt_ndinit(btp, btp->cbtnp);
		errno = BTENKEY;
		return -1;
	}

	/* read current node */
	if (bt_ndget(btp, btp->cbtpos.node, btp->cbtnp) == -1) {
		BTEPRINT;
		terrno = errno;
		btp->cbtpos.node = NIL;
		btp->cbtpos.key = 0;
		bt_ndinit(btp, btp->cbtnp);
		errno = terrno;
		return -1;
	}

	return 0;
}
