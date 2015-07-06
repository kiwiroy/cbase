/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lsgetrf.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <blkio.h>

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     lsgetrf - get field from current lseq record

SYNOPSIS
     #include <lseq.h>

     int lsgetrf(lsp, offset, buf, bufsize)
     lseq_t *lsp;
     size_t offset;
     void *buf;
     size_t bufsize;

DESCRIPTION
     The lsgetrf function reads a field from the current record in
     lseq lsp into buf.  The field begins offset characters from the
     beginning of the record and is bufsize characters long.  buf must
     point to a storage area at least bufsize characters long.

     lsgetrf will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [EINVAL]       buf is the NULL pointer.
     [EINVAL]       bufsize is less than 1.
     [LSEBOUND]     offset + bufsize extends beyond the end of the
                    record.
     [LSELOCK]      lsp is not read locked.
     [LSENOPEN]     lsp is not open.
     [LSENREC]      The cursor is null.

SEE ALSO
     lscursor, lsgetr, lsputrf.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lsgetrf(lseq_t *lsp, size_t offset, void *buf, size_t bufsize)
#else
int lsgetrf(lsp, offset, buf, bufsize)
lseq_t *lsp;
size_t offset;
void *buf;
size_t bufsize;
#endif
{
	/* validate arguments */
	if (!ls_valid(lsp) || buf == NULL || bufsize < 1) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(lsp->flags & LSOPEN)) {
		errno = EINVAL;
		return -1;
	}

	/* check if not read locked */
	if (!(lsp->flags & LSRDLCK)) {
		errno = LSELOCK;
		return -1;
	}

	/* check if over record boundary */
	if (offset + bufsize > lsp->lshdr.recsize) {
		errno = LSEBOUND;
		return -1;
	}

	/* check if cursor is null */
	if (lsp->clspos == NIL) {
		errno = LSENREC;
		return -1;
	}

	/* copy field from current record */
	memcpy(buf, (char *)lsp->clsrp->recbuf + offset, bufsize);

	return 0;
}
