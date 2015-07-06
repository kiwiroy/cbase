/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbrecali.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDLIB
#include <stdlib.h>
#endif
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <blkio.h>
#include <btree.h>
#include <lseq.h>

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cbrecalign - align cbase record cursor

SYNOPSIS
     #include <cbase.h>

     int cbrecalign(cbp, field)
     cbase_t *cbp;
     int field;

DESCRIPTION
     The cbrecalign function aligns the record cursor in cbase cbp
     with the key cursor for the specified field.  Other cursors are
     not affected.

     cbrecalign will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [EINVAL]       field is not a valid field number for
                    cbase cbp.
     [CBELOCK]      cbp is not locked.
     [CBENKEY]      field is not a key.
     [CBENOPEN]     cbp is not open.

SEE ALSO
     cbkeyalign, cbrecfirst, cbreclast, cbrecnext, cbrecprev.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbrecalign(cbase_t *cbp, int field)
#else
int cbrecalign(cbp, field)
cbase_t *cbp;
int field;
#endif
{
	void *	buf	= NULL;
	cbrpos_t cbrpos	= NIL;
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

	/* validate arguments */
	if (field < 0 || field >= cbp->fldc) {
		errno = EINVAL;
		return -1;
	}

	/* check if field not a key */
	if (!(cbp->fldv[field].flags & CB_FKEY)) {
		errno = CBENKEY;
		return -1;
	}

	/* check if not locked */
	if (!(cbp->flags & CBLOCKS)) {
		errno = CBELOCK;
		return -1;
	}

	/* check if record cursor is null */
	if (btcursor(cbp->btpv[field]) == NULL) {
		/* set record cursor to null */
		if (lssetcur(cbp->lsp, NULL) == -1) {
			CBEPRINT;
			return -1;
		}
		return 0;
	}

	/* allocate storage for key */
	if (btkeysize(cbp->btpv[field]) != (cbp->fldv[field].len + sizeof(cbrpos_t))) {
		CBEPRINT;
		errno = CBEPANIC;
		return -1;
	}
	buf = calloc((size_t)1, btkeysize(cbp->btpv[field]));
	if (buf == NULL) {
		CBEPRINT;
		errno = ENOMEM;
		return -1;
	}

	/* get key */
	if (btgetk(cbp->btpv[field], buf) == -1) {
		CBEPRINT;
		free(buf);
		return -1;
	}

	/* get record position and place in key */
	memcpy(&cbrpos, (char *)buf + cbp->fldv[field].len, sizeof(cbrpos));
	lspos = cbrpos;

	/* position record cursor */
	if (lssetcur(cbp->lsp, &lspos) == -1) {
		CBEPRINT;
		return -1;
	}

	free(buf);
	buf = NULL;

	return 0;
}
