/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbclose.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <btree.h>
#include <lseq.h>

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cbclose - close cbase

SYNOPSIS
     #include <cbase.h>

     int cbclose(cbp)
     cbase_t *cbp;

DESCRIPTION
     The cbclose function causes any buffered data for cbase cbp to be
     written out and the cbase to be unlocked and closed.

     cbclose will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [CBENOPEN]     cbp is not open.

SEE ALSO
     cbcreate, cbopen, cbsync.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbclose(cbase_t *cbp)
#else
int cbclose(cbp)
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

	/* flush buffers and unlock file */
	if (cblock(cbp, CB_UNLCK) == -1) {
		CBEPRINT;
		return -1;
	}

	/* close record file */
	if (lsclose(cbp->lsp) == -1) {
		CBEPRINT;
		return -1;
	}

	/* close key files */
	for (i = 0; i < cbp->fldc; ++i) {
		if (cbp->fldv[i].flags & CB_FKEY) {
			if (btclose(cbp->btpv[i]) == -1) {
				CBEPRINT;
				return -1;
			}
		}
	}

	/* free memory allocated for cbase */
	cb_freemem(cbp);

	/* scrub slot in cbb table then free it */
	memset(cbp, 0, sizeof(*cbb));
	cbp->flags = 0;

	return 0;
}
