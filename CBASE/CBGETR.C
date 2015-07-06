/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbgetr.c	1.5 - 91/09/23" */

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
     cbgetr - get cbase record

SYNOPSIS
     #include <cbase.h>

     int cbgetr(cbp, buf)
     cbase_t *cbp;
     void *buf;

DESCRIPTION
     The cbgetr function reads the current record of cbase cbp into
     buf.  buf must point to a storage area at least as large as the
     record size for cbp.

     cbgetr will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [EINVAL]       buf is the NULL pointer.
     [CBELOCK]      cbp is not read locked.
     [CBENOPEN]     cbp is not open.
     [CBENREC]      The record cursor for cbp is null.

SEE ALSO
     cbrcursor, cbrecsize, cbgetrf, cbputr.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbgetr(cbase_t *cbp, void *buf)
#else
int cbgetr(cbp, buf)
cbase_t *cbp;
void *buf;
#endif
{
	/* validate arguments */
	if (!cb_valid(cbp) || buf == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(cbp->flags & CBOPEN)) {
		errno = CBENOPEN;
		return -1;
	}

	/* check if not read locked */
	if (!(cbp->flags & CBRDLCK)) {
		errno = CBELOCK;
		return -1;
	}

	/* check if cursor is null */
	if (lscursor(cbp->lsp) == NULL) {
		errno = CBENREC;
		return -1;
	}

	/* read field */
	if (lsgetr(cbp->lsp, buf) == -1) {
		CBEPRINT;
		return -1;
	}

	return 0;
}
