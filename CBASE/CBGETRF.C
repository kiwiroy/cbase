/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbgetrf.c	1.5 - 91/09/23" */

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
     cbgetrf - get cbase record field

SYNOPSIS
     #include <cbase.h>

     int cbgetrf(cbp, field, buf)
     cbase_t *cbp;
     int field;
     void *buf;

DESCRIPTION
     The cbgetrf function reads the specified field from the current
     record of cbase cbp into buf.  buf must point to a storage area
     of size no less than the size of the specified field.

     cbgetrf will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [EINVAL]       field is not a valid field number for
                    cbase cbp.
     [EINVAL]       buf is the NULL pointer.
     [CBELOCK]      cbp is not read locked.
     [CBENOPEN]     cbp is not open.
     [CBENREC]      The record cursor for cbp is null.

SEE ALSO
     cbcursor, cbgetr, cbputrf.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbgetrf(cbase_t *cbp, int field, void *buf)
#else
int cbgetrf(cbp, field, buf)
cbase_t *cbp;
int field;
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

	/* validate field argument */
	if (field < 0 || field >= cbp->fldc) {
		errno = EINVAL;
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
	if (lsgetrf(cbp->lsp, cbp->fldv[field].offset, buf, cbp->fldv[field].len) == -1) {
		CBEPRINT;
		return -1;
	}

	return 0;
}
