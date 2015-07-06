/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbputr.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* library headers */
#include <lseq.h>

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cbputr - put cbase record

SYNOPSIS
     #include <cbase.h>

     int cbputr(cbp, buf)
     cbase_t *cbp;
     const void *buf;

DESCRIPTION
     The cbputr function overwrites the current record of cbase cbp.
     The record cursor and all key cursors are positioned to the
     modified record.

     If the new record contents cannot be inserted because of an
     illegal duplicate key, the original record is not restored.
     More complete control of the actions taken to exceptions such as
     this can be gained by overwriting a record using a series of
     function calls to search, delete, and insert, rather than a
     single call to cbputr.

     cbputr will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [EINVAL]       buf is the NULL pointer.
     [CBEDUP]       A field in the record pointed to by buf contains
                    an illegal duplicate key.
     [CBELOCK]      cbp is not write locked.
     [CBENOPEN]     cbp is not open.
     [CBENREC]      The record cursor of cbp is null.

SEE ALSO
     cbdelcur, cbgetr, cbinsert, cbrcursor.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbputr(cbase_t *cbp, const void *buf)
#else
int cbputr(cbp, buf)
cbase_t *cbp;
const void *buf;
#endif
{
	/* validate arguments */
	if (!cb_valid(cbp)) {
		errno = EINVAL;
		return -1;
	}
	if (buf == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(cbp->flags & CBOPEN)) {
		errno = CBENOPEN;
		return -1;
	}

	/* check if not write locked */
	if (!(cbp->flags & CBWRLCK)) {
		errno = CBELOCK;
		return -1;
	}

	/* check if record cursor is null */
	if (lscursor(cbp->lsp) == NULL) {
		errno = CBENREC;
		return -1;
	}

	/* delete current record */
	if (cbdelcur(cbp) == -1) {
		CBEPRINT;
		return -1;
	}

	/* back up cursor */
	if (cbrecprev(cbp) == -1) {
		CBEPRINT;
		return -1;
	}

	/* insert new record */
	if (cbinsert(cbp, buf) == -1) {
		CBEPRINT;
		return -1;
	}

	return 0;
}
