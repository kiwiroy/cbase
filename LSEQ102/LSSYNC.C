/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lssync.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* library headers */
#include <blkio.h>

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     lssync - lseq synchronize

SYNOPSIS
     #include <lseq.h>

     int lssync(lsp);
     lseq_t *lsp;

DESCRIPTION
     The lssync function causes any buffered data for the named lseq
     to be written to the file.  The lseq remains open and the buffer
     contents remain intact.

     lssync will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [LSENOPEN]     lsp is not open.

SEE ALSO
     lsclose, lslock, lssetbuf, lssetvbuf.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lssync(lseq_t *lsp)
#else
int lssync(lsp)
lseq_t *lsp;
#endif
{
	/* validate arguments */
	if (!ls_valid(lsp)) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(lsp->flags & LSOPEN)) {
		errno = LSENOPEN;
		return -1;
	}

	/* synchronize file with buffers */
	if (bsync(lsp->bp) == -1) {
		LSEPRINT;
		return -1;
	}

	return 0;
}
