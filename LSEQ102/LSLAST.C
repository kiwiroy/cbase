/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lslast.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     lslast - last lseq record

SYNOPSIS
     #include <lseq.h>

     int lslast(lsp)
     lseq_t *lsp;

DESCRIPTION
     The lslast function positions the cursor of lseq lsp on the last
     record in that lseq.

     lslast will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [LSELOCK]      lsp is not locked.
     [LSENOPEN]     lsp is not open.
     [LSENREC]      lsp is empty.

SEE ALSO
     lsfirst, lsnext, lsprev, lsreccnt.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lslast(lseq_t *lsp)
#else
int lslast(lsp)
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

	/* check locks */
	if (!(lsp->flags & LSLOCKS)) {
		errno = LSELOCK;
		return -1;
	}

	/* check if lsp is empty */
	if (lsp->lshdr.reccnt == NIL) {
		errno = LSENREC;
		return -1;
	}

	/* check if already on last record */
	if (lsp->clspos == lsp->lshdr.last) {
		return 0;
	}

	/* set cursor to last record */
	lsp->clspos = lsp->lshdr.last;

	/* read in last record */
	if (lsp->clspos == NIL) {
		LSEPRINT;
		errno = LSEPANIC;
		return -1;
	} else {
		if (ls_rcget(lsp, lsp->clspos, lsp->clsrp) == -1) {
			LSEPRINT;
			return -1;
		}
	}

	return 0;
}
