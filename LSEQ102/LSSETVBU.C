/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lssetvbu.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif

/* local headers */
#include <blkio.h>

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     lssetvbuf - assign buffering to an lseq

SYNOPSIS
     #include <lseq.h>

     int lssetvbuf(lsp, buf, bufcnt)
     lseq_t *lsp;
     void *buf;
     size_t bufcnt;

DESCRIPTION
     The lssetvbuf function is used to assign buffering to an lseq.
     bufcnt specifies the number of lseq records to be buffered.  If
     bufcnt has a value of zero, the lseq will be completely
     unbuffered.  If buf is not the NULL pointer, the storage area it
     points to will be used instead of one automatically allocated for
     buffering.

     The size of the storage area needed can be obtained using the
     LSBUFSIZE() macro:

          char buf[LSBUFSIZE(RECSIZE, BUFCNT)];
          lssetvbuf(lsp, buf, BUFCNT);

     where RECSIZE is the size of the keys int the lseq, and BUFCNT is
     the number of records to buffer.

     Any previously buffered data is flushed before installing the new
     buffer area, so lssetvbuf may be called more than once.  This
     allows the buffer size to be varied with the file size.

     lssetvbuf will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [LSENOPEN]     lsp is not open.

SEE ALSO
     lssetbuf, lssync.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lssetvbuf(lseq_t *lsp, void *buf, size_t bufcnt)
#else
int lssetvbuf(lsp, buf, bufcnt)
lseq_t *lsp;
void *buf;
size_t bufcnt;
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

	/* set buffering */
	if (bsetvbuf(lsp->bp, buf, ls_blksize(lsp), bufcnt) == -1) {
		LSEPRINT;
		return -1;
	}

	return 0;
}
