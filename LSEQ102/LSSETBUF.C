/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lssetbuf.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* library headers */
#include <blkio.h>

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     lssetbuf - assign buffering to an lseq

SYNOPSIS
     #include <lseq.h>

     int lssetbuf(lsp, buf)
     lseq_t *lsp;
     void *buf;

DESCRIPTION
     The lssetbuf function causes the storage area pointed to by buf
     to be used by the lseq associated with lseq pointer lsp instead
     of an automatically allocated buffer area.  If buf is the NULL
     pointer, lsp will be completely unbuffered.

     The size of the storage area needed can be obtained using the
     LSBUFSIZE() macro:

          char buf[LSBUFSIZE(RECSIZE, LSBUFCNT)];
          lssetbuf(lsp, (void *)buf);

     where RECSIZE is the size of the records in the lseq and LSBUFCNT
     is the default number of records buffered when an lseq is opened.
     LSBUFCNT is defined in <lseq.h>.  If the number of records
     buffered has been changed using lssetvbuf, then that number
     should be used in place of LSBUFCNT.

     lssetbuf may be called at any time after opening the lseq,
     before and after it is read or written; the buffers are flushed
     before installing the new buffer area.

     lssetbuf will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [LSENBUF]      buf is not the NULL pointer and lsp
                    is not buffered.
     [LSENOPEN]     lsp is not open.

SEE ALSO
     lssetvbuf, lssync.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lssetbuf(lseq_t *lsp, void *buf)
#else
int lssetbuf(lsp, buf)
lseq_t *lsp;
void *buf;
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

	/* assign buffering */
	if (bsetbuf(lsp->bp, buf) == -1) {
		if (errno == BENBUF) errno = LSENBUF;
		LSEPRINT;
		return -1;
	}

	return 0;
}
