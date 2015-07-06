/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lsclose.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <blkio.h>

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     lsclose - close lseq

SYNOPSIS
     #include <lseq.h>

     int lsclose(lsp)
     lseq_t *lsp;

DESCRIPTION
     The lsclose function causes any buffered data for the named lseq
     to be written out, the file unlocked, and the lseq closed.

     lsclose will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [LSENOPEN]     lsp is not open.

SEE ALSO
     lscreate, lslock, lsopen, lssync.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lsclose(lseq_t *lsp)
#else
int lsclose(lsp)
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

	/* synchronize file with buffers and unlock file */
	if (lslock(lsp, LS_UNLCK) == -1) {
		LSEPRINT;
		return -1;
	}

	/* free memory allocated for lsp */
	ls_free(lsp);

	/* close lseq file */
	if (bclose(lsp->bp) == -1) {
		LSEPRINT;
		return -1;
	}

	/* scrub slot in lsb table then free it */
	memset(lsp, 0, sizeof(*lsb));
	lsp->flags = 0;

	return 0;
}
