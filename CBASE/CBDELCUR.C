/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbdelcur.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#ifdef AC_STDLIB
#include <stdlib.h>
#endif
#ifdef AC_STRING
#include <string.h>
#endif

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cbdelcur - delete current cbase record

SYNOPSIS
     #include <cbase.h>

     int cbdelcur(cbp)
     cbase_t *cbp;

DESCRIPTION
     The cbdelcur function deletes the current record of cbase cbp.
     The record cursor is set to the record following the deleted
     record.  All key cursors are set to null.

     cbdelcur will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [CBELOCK]      cbp is not write locked.
     [CBENOPEN]     cbp is not open.
     [CBENREC]      The record cursor of cbp is null.

SEE ALSO
     cbinsert, cbrcursor.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbdelcur(cbase_t * cbp)
#else
int cbdelcur(cbp)
cbase_t * cbp;
#endif
{
	lspos_t	lspos	= NIL;
	cbrpos_t cbrpos	= NIL;
	void *	buf	= NULL;
	int	i	= 0;

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

	/* get record position */
	if (lsgetcur(cbp->lsp, &lspos) == -1) {
		CBEPRINT;
		return -1;
	}
	cbrpos = lspos;

	/* delete keys */
	for (i = 0; i < cbp->fldc; ++i) {
		if (cbp->fldv[i].flags & CB_FKEY) {
			buf = calloc((size_t)1, cbp->fldv[i].len + sizeof(cbrpos_t));
			if (buf == NULL) {
				CBEPRINT;
				errno = ENOMEM;
				return -1;
			}
			if (lsgetrf(cbp->lsp, cbp->fldv[i].offset, buf, cbp->fldv[i].len) == -1) {
				CBEPRINT;
				free(buf);
				return -1;
			}
			memcpy((char *)buf + cbp->fldv[i].len, &cbrpos, sizeof(cbrpos_t));
			if (btdelete(cbp->btpv[i], buf) == -1) {
				CBEPRINT;
				free(buf);
				return -1;
			}
			free(buf);
			buf = NULL;
		}
	}

	/* delete current record */
	if (lsdelcur(cbp->lsp) == -1) {
		CBEPRINT;
		return -1;
	}

	return 0;
}
