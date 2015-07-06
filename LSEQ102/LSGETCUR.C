/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lsgetcur.c	1.5 - 91/09/23" */

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
     lsgetcur - get lseq cursor

SYNOPSIS
     #include <lseq.h>

     int lsgetcur(lsp, lsposp)
     lseq_t *lsp;
     lspos_t *lsposp;

DESCRIPTION
     The lsgetcur function gets the current cursor position of lseq
     lsp and copies it to the location pointed to by lsposp.  A lseq
     position saved with lsgetcur can be used to reposition to a
     record using lssetcur.  It is important to remember that an lseq
     position saved with lsgetcur is not valid after that lseq has
     been unlocked.

     lsgetcur will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [EINVAL]       lsposp is the NULL pointer.
     [LSELOCK]      lsp is not locked.
     [LSENOPEN]     lsp is not open.

SEE ALSO
     lssetcur.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lsgetcur(lseq_t *lsp, lspos_t *lsposp)
#else
int lsgetcur(lsp, lsposp)
lseq_t *lsp;
lspos_t *lsposp;
#endif
{
	/* validate arguments */
	if (!ls_valid(lsp) || lsposp == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(lsp->flags & LSOPEN)) {
		errno = EINVAL;
		return -1;
	}

	/* check if not read locked */
	if (!(lsp->flags & LSLOCKS)) {
		errno = LSELOCK;
		return -1;
	}

	/* get current position */
	memcpy(lsposp, &lsp->clspos, sizeof(lsp->clspos));

	return 0;
}
