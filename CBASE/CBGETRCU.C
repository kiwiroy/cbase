/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbgetrcu.c	1.5 - 91/09/23" */

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
     cbgetrcur - get cbase record cursor

SYNOPSIS
     #include <cbase.h>

     int cbgetrcur(cbp, cbrposp)
     cbase_t *cbp;
     cbrpos_t *cbrposp;

DESCRIPTION
     The cbgetrcur function gets the position of the record cursor of
     cbase cbp.  The cursor position is returned in the location
     pointed to by cbrposp.

     cbgetrcur will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [EINVAL]       cbrposp is the NULL pointer.
     [CBELOCK]      cbp is not locked.
     [CBENOPEN]     cbp is not open.

SEE ALSO
     cbgetkcur, cbrcursor, cbsetrcur.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbgetrcur(cbase_t *cbp, cbrpos_t *cbrposp)
#else
int cbgetrcur(cbp, cbrposp)
cbase_t *cbp;
cbrpos_t *cbrposp;
#endif
{
	lspos_t lspos = NIL;

	/* validate arguments */
	if (!cb_valid(cbp) || cbrposp == NULL) {
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

	/* get record cursor position */
	if (lsgetcur(cbp->lsp, &lspos) == -1) {
		CBEPRINT;
		return -1;
	}

	/* load return argument */
	*cbrposp = lspos;

	return 0;
}
