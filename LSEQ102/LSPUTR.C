/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lsputr.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#ifdef AC_STDDEF
#include <stddef.h>
#endif

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     lsputr - put current lseq record

SYNOPSIS
     #include <lseq.h>

     int lsputr(lsp, buf)
     lseq_t *lsp;
     const void *buf;

DESCRIPTION
     The lsputr function writes the record pointed to by buf into the
     current record in lseq lsp.  buf must point to a storage area at
     least as large as the record size for lsp.

     lsputr will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [EINVAL]       buf is the NULL pointer.
     [LSELOCK]      lsp is not write locked.
     [LSENOPEN]     lsp is not open.
     [LSENREC]      The cursor is null.

SEE ALSO
     lscursor, lsgetr, lsputrf.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lsputr(lseq_t *lsp, const void *buf)
#else
int lsputr(lsp, buf)
lseq_t *lsp;
const void *buf;
#endif
{
	return lsputrf(lsp, (size_t)0, buf, lsp->lshdr.recsize);
}
