/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lssetcur.c	1.5 - 91/09/23" */

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
     lssetcur - set lseq cursor

SYNOPSIS
     #include <lseq.h>

     int lssetcur(lsp, lsposp)
     lseq_t *lsp;
     const lspos_t *lsposp;

DESCRIPTION
     The lssetcur function sets the current cursor position of lseq
     lsp to the value pointed to by lsposp.  lsposp should point to a
     cursor value saved previously with lsgetcur.  If lsposp is the
     NULL pointer, the cursor is set to null.  It is important to
     remember that an lseq position saved with lsgetcur is not valid
     after that lseq has been unlocked.

     lssetcur will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [LSELOCK]      lsp is not locked.
     [LSENOPEN]     lsp is not open.

SEE ALSO
     lsgetcur.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lssetcur(lseq_t *lsp, const lspos_t *lsposp)
#else
int lssetcur(lsp, lsposp)
lseq_t *lsp;
const lspos_t *lsposp;
#endif
{
	/* validate arguments */
	if (!ls_valid(lsp)) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(lsp->flags & LSOPEN)) {
		errno = EINVAL;
		return -1;
	}

	/* check locks */
	if (!(lsp->flags & LSLOCKS)) {
		errno = LSELOCK;
		return -1;
	}

	/* set new currency */
	if (lsposp == NULL) {
		lsp->clspos = NIL;	/* set cursor to null */
	} else {
		memcpy(&lsp->clspos, lsposp, sizeof(lsp->clspos));
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