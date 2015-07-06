/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lsgetr.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#ifdef AC_STDDEF
#include <stddef.h>
#endif

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     lsgetr - get current lseq record

SYNOPSIS
     #include <lseq.h>

     int lsgetr(lsp, buf)
     lseq_t *lsp;
     void *buf;

DESCRIPTION
     The lsgetr function reads the record from the current cursor
     position in lseq lsp into buf.  buf must point to a storage area
     as large as the record size for lsp.

     lsgetr will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [EINVAL]       buf is the NULL pointer.
     [LSELOCK]      lsp is not read locked.
     [LSENOPEN]     lsp is not open.
     [LSENREC]      The cursor is null.

SEE ALSO
     lscursor, lsgetrf, lsputr.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lsgetr(lseq_t *lsp, void *buf)
#else
int lsgetr(lsp, buf)
lseq_t *lsp;
void *buf;
#endif
{
	return lsgetrf(lsp, (size_t)0, buf, lsp->lshdr.recsize);
}
