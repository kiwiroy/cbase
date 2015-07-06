/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbrecpre.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* library headers */
#include <lseq.h>

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cbrecprev - previous cbase record

SYNOPSIS
     #include <cbase.h>

     int cbrecprev(cbp)
     cbase_t *cbp;

DESCRIPTION
     The cbrecprev function retreats the record cursor of cbase cbp to
     the previous record.  If the cursor is on the first record, it is
     retreated to null.

     cbrecprev will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [CBELOCK]      cbp is not locked.
     [CBENOPEN]     cbp is not open.

SEE ALSO
     cbrcursor, cbrecfirst, cbreclast, cbrecnext.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbrecprev(cbase_t *cbp)
#else
int cbrecprev(cbp)
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

	/* retreat cursor to previous record */
	if (lsprev(cbp->lsp) == -1) {
		CBEPRINT;
		return -1;
	}

	return 0;
}
