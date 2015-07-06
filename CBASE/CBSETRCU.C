/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbsetrcu.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* library headers */
#include <blkio.h>
#include <lseq.h>

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cbsetrcur - set cbase record cursor

SYNOPSIS
     #include <cbase.h>

     int cbsetrcur(cbp, cbrposp)
     cbase_t *cbp;
     const cbrpos_t *cbrposp;

DESCRIPTION
     The cbsetrcur function sets the position of the record cursor of
     cbase cbp to the value pointed to by cbposp.  If cbrposp is the
     NULL pointer, the record cursor is set to null.

     cbsetrcur will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [CBELOCK]      cbp is not locked.
     [CBENOPEN]     cbp is not open.

SEE ALSO
     cbgetrcur, cbrcursor, cbsetkcur.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbsetrcur(cbase_t *cbp, const cbrpos_t *cbrposp)
#else
int cbsetrcur(cbp, cbrposp)
cbase_t *cbp;
const cbrpos_t *cbrposp;
#endif
{
	lspos_t	lspos	= NIL;

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

	/* set record cursor position */
	if (cbrposp == NULL) {
		if (lssetcur(cbp->lsp, NULL) == -1) {
			CBEPRINT;
			return -1;
		}
	} else {
		lspos = *cbrposp;
		if (lssetcur(cbp->lsp, &lspos) == -1) {
			CBEPRINT;
			return -1;
		}
	}

	return 0;
}
