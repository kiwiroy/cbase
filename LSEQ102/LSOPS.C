/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lsops.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDLIB
#include <stdlib.h>
#endif

/* non-ansi headers */
#include <bool.h>

/* library headers */
#include <blkio.h>

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     ls_alloc - allocate memory for a lseq

SYNOPSIS
     #include "lseq_.h"

     int ls_alloc(lsp);
     lseq_t *lsp;

DESCRIPTION
     The ls_alloc function allocates the memory needed by lseq lsp.
     The memory is initialized to 0.

     ls_alloc will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [ENOMEM]       Not enough memory is available for
                    allocation by the calling process.
     [LSENOPEN]     lsp is not open.

SEE ALSO
     ls_free.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int ls_alloc(lseq_t *lsp)
#else
int ls_alloc(lsp)
lseq_t *lsp;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!ls_valid(lsp)) {
		LSEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(lsp->flags & LSOPEN)) {
		LSEPRINT;
		errno = LSENOPEN;
		return -1;
	}

	/* check for memory leak */
	if (lsp->clsrp != NULL) {
		LSEPRINT;
		errno = LSEPANIC;
		return -1;
	}
#endif
	/* current record */
	lsp->clsrp = ls_rcalloc(lsp);
	if (lsp->clsrp == NULL) {
		LSEPRINT;
		return -1;
	}

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     ls_blksize - lseq block size

SYNOPSIS
     #include <lseq.h>

     size_t ls_blksize(lsp)
     lseq_t *lsp;

DESCRIPTION
     ls_blksize returns the size of the blocks in lseq lsp.  If lsp is
     not a valid open lseq, the results are undefined.  ls_blksize is
     a macro.

------------------------------------------------------------------------------*/
/* ls_blksize defined in lseq_.h. */

/*man---------------------------------------------------------------------------
NAME
     ls_free - free memory allocated for an lseq

SYNOPSIS
     #include "lseq_.h"

     void ls_free(lsp)
     lseq_t *lsp;

DESCRIPTION
     The ls_free function frees all memory allocated for lseq lsp.
     If lsp is not a valid lseq, no action is taken.

SEE ALSO
     ls_alloc.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
void ls_free(lseq_t *lsp)
#else
void ls_free(lsp)
lseq_t *lsp;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!ls_valid(lsp)) {
		LSEPRINT;
		return;
	}
#endif
	/* free memory */
	ls_rcfree(lsp->clsrp);
	lsp->clsrp = NULL;

	return;
}

/*man---------------------------------------------------------------------------
NAME
     ls_valid - validate lseq

SYNOPSIS
     #include "lseq_.h"

     bool ls_valid(lsp)
     lseq_t *lsp;

DESCRIPTION
     The ls_valid function determines if lsp is a valid lseq pointer.
     If valid, then TRUE is returned.  If not, then FALSE is returned.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
bool ls_valid(lseq_t *lsp)
#else
bool ls_valid(lsp)
lseq_t *lsp;
#endif
{
	if (lsp < lsb || lsp > (lsb + LSOPEN_MAX - 1)) {
		return FALSE;
	}
	if ((((char *)lsp - (char *)lsb)) % sizeof(*lsb) != 0) {
		return FALSE;
	}

	return TRUE;
}
