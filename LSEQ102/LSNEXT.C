/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lsnext.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     lsnext - next lseq record

SYNOPSIS
     #include <lseq.h>

     int lsnext(lsp)
     lseq_t *lsp;

DESCRIPTION
     The lsnext function advances the cursor of lseq lsp to the next
     record.  If the cursor is currently null, it will be advanced to
     the first record.  If the cursor is currently on the last record,
     it will be advanced to null.  If lsp is empty, the cursor will
     remain set to null.

     lsnext will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [LSELOCK]      lsp is not locked.
     [LSENOPEN]     lsp is not open.

SEE ALSO
     lscursor, lsfirst, lslast, lsprev.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lsnext(lseq_t *lsp)
#else
int lsnext(lsp)
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

	/* advance cursor */
	if (lsp->clspos == NIL) {
		lsp->clspos = lsp->lshdr.first;
	} else {
		lsp->clspos = lsp->clsrp->next;
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