/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lsfirst.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     lsfirst - first lseq record

SYNOPSIS
     #include <lseq.h>

     int lsfirst(lsp)
     lseq_t *lsp;

DESCRIPTION
     The lsfirst function positions the cursor of lseq lsp on the
     first record in that lseq.

     lsfirst will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [LSELOCK]      lsp is not locked.
     [LSENOPEN]     lsp is not open.
     [LSENREC]      lsp is empty.

SEE ALSO
     lslast, lsnext, lsprev, lsreccnt.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lsfirst(lseq_t *lsp)
#else
int lsfirst(lsp)
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

	/* check if already on first record */
	if (lsp->clspos == lsp->lshdr.first) {
		return 0;
	}

	/* set cursor to first record */
	lsp->clspos = lsp->lshdr.first;

	/* read in first record */
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