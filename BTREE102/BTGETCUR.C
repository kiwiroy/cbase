/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btgetcur.c	1.5 - 91/09/23" */

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
     btgetcur - get btree cursor

SYNOPSIS
     #include <btree.h>

     int btgetcur(btp, btposp)
     btree_t *btp;
     btpos_t *btposp;

DESCRIPTION
     The btgetcur function gets the current cursor position for btree
     btp and copies it to btposp.  A btree position saved with
     btgetcur can be used to reposition to a key using btsetcur.  It
     is important to remember that a btree position is not valid after
     an insertion, deletion, or unlock.

     btgetcur will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       btposp is the NULL pointer.
     [BTELOCK]      btp is not locked.
     [BTENOPEN]     btp is not open.

SEE ALSO
     btsetcur.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btgetcur(btree_t *btp, btpos_t *btposp)
#else
int btgetcur(btp, btposp)
btree_t *btp;
btpos_t *btposp;
#endif
{
	/* validate arguments */
	if (!bt_valid(btp) || btposp == NULL) {
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

	/* get current position */
	memcpy(btposp, &btp->cbtpos, sizeof(*btposp));

	return 0;
}
