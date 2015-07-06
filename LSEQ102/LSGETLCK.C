/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lsgetlck.c	1.5 - 91/09/23" */

#include <ansi.h>

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     lsgetlck - get lseq lock status

SYNOPSIS
     #include <lseq.h>

     int lsgetlck(lsp)
     lseq_t *lsp;

DESCRIPTION
     The lsgetlck function reports the lock status of lseq lsp.  The
     function returns the status of the lock currently held by the
     calling process.  Locks held by other processes are not reported.

     The possible return values are:

          LS_UNLCK     lseq not locked
          LS_RDLCK     lseq locked for reading
          LS_WRLCK     lseq locked for reading and writing

SEE ALSO
     lslock.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lsgetlck(lseq_t *lsp)
#else
int lsgetlck(lsp)
lseq_t *lsp;
#endif
{
	if (!(lsp->flags & LSLOCKS)) {
		return LS_UNLCK;
	} else if (lsp->flags & LSWRLCK) {
		return LS_WRLCK;
	}

	return LS_RDLCK;
}
