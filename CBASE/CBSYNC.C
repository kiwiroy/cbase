/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbsync.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* library headers */
#include <btree.h>
#include <lseq.h>

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cbsync - synchronize cbase

SYNOPSIS
     #include <cbase.h>

     int cbsync(cbp)
     cbase_t *cbp;

DESCRIPTION
     The cbsync function causes any buffered data for the named cbase
     to be written out, both for the record and the key files.  The
     cbase remains open and the buffer contents remain intact.

     cbsync will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [EINVAL]       cbp is not open.

SEE ALSO
     cbclose, cblock.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbsync(cbase_t *cbp)
#else
int cbsync(cbp)
cbase_t *cbp;
#endif
{
	int i = 0;

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

	/* synchronize record file with buffers */
	if (lssync(cbp->lsp) == -1) {
		CBEPRINT;
		return -1;
	}

	/* synchronize key files with buffers */
	for (i = 0; i < cbp->fldc; ++i) {
		if (cbp->fldv[i].flags & CB_FKEY) {
			if (btsync(cbp->btpv[i]) == -1) {
				CBEPRINT;
				return -1;
			}
		}
	}

	return 0;
}
