/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbrecfir.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* library headers */
#include <lseq.h>

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cbrecfirst - first cbase record

SYNOPSIS
     #include <cbase.h>

     int cbrecfirst(cbp)
     cbase_t *cbp;

DESCRIPTION
     The cbrecfirst function positions the record cursor of cbase cbp
     on the first record.

     cbrecfirst will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [CBELOCK]      cbp is not locked.
     [CBENOPEN]     cbp is not open.
     [CBENREC]      cbp is empty.

SEE ALSO
     cbreccnt, cbreclast, cbrecnext, cbrecprev.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbrecfirst(cbase_t *cbp)
#else
int cbrecfirst(cbp)
cbase_t *cbp;
#endif
{
	/* validate arguments */
	if (!cb_valid(cbp)) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(cbp->flags & CBOPEN)) {
		errno = CBENOPEN;
		return -1;
	}

	/* check if not locked */
	if (!(cbp->flags & CBLOCKS)) {
		errno = CBELOCK;
		return -1;
	}

	/* check if cbp is empty */
	if (lsreccnt(cbp->lsp) == 0) {
		errno = CBENREC;
		return -1;
	}

	/* set cursor to first record */
	if (lsfirst(cbp->lsp) == -1) {
		CBEPRINT;
		return -1;
	}

	return 0;
}
