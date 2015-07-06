/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lsprev.c	1.5 - 91/09/23" */

/* ansi headers */
#include <errno.h>

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     lsprev - previous lseq record

SYNOPSIS
     #include <lseq.h>

     int lsprev(lsp)
     lseq_t *lsp;

DESCRIPTION
     The lsprev function retreats the cursor of lseq lsp to the
     previous record.  If the cursor is currently null, it will be
     moved to the last record.  If the cursor is currently on the last
     record, it will be moved to null.  If lsp is empty, the cursor
     will remain set to null.

     lsprev will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [LSELOCK]      lsp is not locked.
     [LSENOPEN]     lsp is not open.

SEE ALSO
     lscursor, lsfirst, lslast, lsnext.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lsprev(lseq_t *lsp)
#else
int lsprev(lsp)
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

	/* check if not locked */
	if (!(lsp->flags & LSLOCKS)) {
		errno = LSELOCK;
		return -1;
	}

	/* move cursor */
	if (lsp->clspos == NIL) {
		lsp->clspos = lsp->lshdr.last;
	} else {
		lsp->clspos = lsp->clsrp->prev;
	}

	/* read in new current record */
	if (lsp->clspos == NIL) {
		ls_rcinit(lsp, lsp->clsrp);
	} else {
		if (ls_rcget(lsp, lsp->clspos, lsp->clsrp) == -1) {
			LSEPRINT;
			return -1;
		}
	}

	return 0;
}
