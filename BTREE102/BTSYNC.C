/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btsync.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* library headers */
#include <blkio.h>

/* local headers */
#include "btree_.h"

/*man---------------------------------------------------------------------------
NAME
     btsync - btree synchronize

SYNOPSIS
     #include <btree.h>

     int btsync(btp);
     btree_t *btp;

DESCRIPTION
     The btsync function causes any buffered data for the named btree
     to be written to the file.  The btree remains open and the buffer
     contents remain intact.

     btsync will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [BTENOPEN]     btp is not open.

SEE ALSO
     btclose, btlock, btsetbuf, btsetvbuf.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btsync(btree_t *btp)
#else
int btsync(btp)
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

	/* synchronize file with buffers */
	if (bsync(btp->bp) == -1) {
		BTEPRINT;
		return -1;
	}

	return 0;
}
