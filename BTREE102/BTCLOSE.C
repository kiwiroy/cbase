/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btclose.c	1.5 - 91/09/23" */

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
     btclose - close btree

SYNOPSIS
     #include <btree.h>

     int btclose(btp)
     btree_t *btp;

DESCRIPTION
     The btclose function causes any buffered data for the named btree
     to be written out, the file unlocked, and the btree to be closed.

     btclose will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [BTENOPEN]     btp is not open.

SEE ALSO
     btcreate, btlock, btopen, btsync.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btclose(btree_t *btp)
#else
int btclose(btp)
btree_t *btp;
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

	/* synchronize file with buffers and unlock file */
	if (btlock(btp, BT_UNLCK) == -1) {
		BTEPRINT;
		return -1;
	}

	/* free memory allocated for btree */
	bt_free(btp);

	/* close btree file */
	if (bclose(btp->bp) == -1) {
		BTEPRINT;
		return -1;
	}

	/* scrub slot in btb table then free it */
	memset(btp, 0, sizeof(*btb));
	btp->flags = 0;

	return 0;
}
