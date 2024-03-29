/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbkeypre.c	1.5 - 91/09/23" */

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
     cbkeyprev - previous cbase key

SYNOPSIS
     #include <cbase.h>

     int cbkeyprev(cbp, field)
     cbase_t *cbp;
     int field;

DESCRIPTION
     The cbkeyprev function retreats the key cursor of the specified
     field in cbase cbp by one.  The record cursor of cbp is
     positioned to the record associated with that key.  Other key
     cursors are not affected.

     cbkeyprev will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [EINVAL]       field is not a valid field number for
                    cbase cbp.
     [CBELOCK]      cbp is not locked.
     [CBENKEY]      field is not a key.
     [CBENOPEN]     cbp is not open.

SEE ALSO
     cbkcursor, cbkeyfirst, cbkeylast, cbkeynext, cbrecprev.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbkeyprev(cbase_t *cbp, int field)
#else
int cbkeyprev(cbp, field)
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

	/* set key cursor to previous key */
	if (btprev(cbp->btpv[field]) == -1) {
		CBEPRINT;
		return -1;
	}

	/* check if cursor is null */
	if (btcursor(cbp->btpv[field]) == NULL) {
		/* set record cursor to null */
		if (lssetcur(cbp->lsp, NULL) == -1) {
			CBEPRINT;
			return -1;
		}
		return 0;
	}

	/* get record position */
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
	if (btgetk(cbp->btpv[field], buf) == -1) {
		CBEPRINT;
		free(buf);
		return -1;
	}
	memcpy(&cbrpos, (char *)buf + cbp->fldv[field].len, sizeof(cbrpos));
	free(buf);
	buf = NULL;

	/* set record cursor */
	lspos = cbrpos;
	if (lssetcur(cbp->lsp, &lspos) == -1) {
		CBEPRINT;
		return -1;
	}

	return 0;
}
