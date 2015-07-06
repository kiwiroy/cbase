/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lssearch.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#ifdef AC_STRING
#include <string.h>
#endif

/* non-ansi headers */
#include <bool.h>

/* library headers */
#include <blkio.h>

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     lssearch - lseq search

SYNOPSIS
     #include <lseq.h>

     int lssearch(lsp, offset, buf, bufsize, cmp)
     lseq_t *lsp;
     size_t offset;
     const void *buf;
     size_t bufsize;
     int (*cmp)(const void *p1, const void *p2, size_t n);

DESCRIPTION
     The lssearch function performs a linear search through lseq lsp
     for a record with a field matching the field pointed to by buf.
     The field being searched begins offset characters from the
     beginning of the record and is bufsize characters long.  The
     function pointed to by cmp is used to test for a match.  The
     search begins on the record following the current record, so
     before the first call to lssearch the cursor should be set to
     null, and successive calls will find succesive records with
     fields matching buf.  If the field is matched, the cursor is left
     on the record with the matching field.

     The user supplied comparison function cmp must be of the
     following form:

          int cmp(const void *p1, const void *p2, size_t n);

     where p1 and p2 point to the two keys to bey compared and n is
     the key size.  The return must be less than, equal to, or greater
     than zero if p1 is less than, equal to, or greater than p2,
     respectively.  If cmp is NULL then the memcmp function is used as
     the default.

     lssearch will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [EINVAL]       buf is the NULL pointer.
     [EINVAL]       bufsize is less than 1.
     [LSEBOUND]     offset + bufsize extends beyond the
                    end of the record.
     [LSELOCK]      lsp is not read locked.
     [LSENOPEN]     lsp is not open.

SEE ALSO
     lscursor.

DIAGNOSTICS
     Upon successful completion, a value of 1 is returned if the field
     was matched or a value of 0 if it was not.  Otherwise, a value of
     -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lssearch(lseq_t *lsp, size_t offset, const void *buf, size_t bufsize, lscmp_t cmp)
#else
int lssearch(lsp, offset, buf, bufsize, cmp)
lseq_t *lsp;
size_t offset;
const void *buf;
size_t bufsize;
lscmp_t cmp;
#endif
{
	bool found = FALSE;

	/* validate arguments */
	if (!ls_valid(lsp) || buf == NULL || bufsize < 1) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(lsp->flags & LSOPEN)) {
		errno = LSENOPEN;
		return -1;
	}

	/* check if not read locked */
	if (!(lsp->flags & LSRDLCK)) {
		errno = LSELOCK;
		return -1;
	}

	/* check if over record boundary */
	if ((offset + bufsize) > lsp->lshdr.recsize) {
		errno = LSEBOUND;
		return -1;
	}

	/* check if cmp is NULL */
	if (cmp == NULL) {
		cmp = memcmp;
	}

	/* advance cursor one record */
	if (lsnext(lsp) == -1) {
		LSEPRINT;
		return -1;
	}

	while (lsp->clspos != NIL) {
		if ((*cmp)(((char *)lsp->clsrp->recbuf + offset), buf, bufsize) == 0) {
			found = TRUE;
			break;
		}
		if (lsnext(lsp) == -1) {
			LSEPRINT;
			return -1;
		}
	}

	return found ? 1 : 0;
}
