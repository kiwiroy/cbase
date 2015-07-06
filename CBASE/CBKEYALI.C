/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbkeyali.c	1.5 - 91/09/23" */

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
     cbkeyalign - align cbase key cursor

SYNOPSIS
     #include <cbase.h>

     int cbkeyalign(cbp, field)
     cbase_t *cbp;
     int field;

DESCRIPTION
     The cbkeyalign function aligns a key cursor in cbase cbp with the
     record cursor.  The key cursor is positioned to the key
     associated with the current record.  Other cursors are not
     affected.

     cbkeyalign will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [EINVAL]       field is not a valid field number for
                    cbase cbp.
     [CBELOCK]      cbp is not locked.
     [CBENKEY]      field is not a key.
     [CBENOPEN]     cbp is not open.

SEE ALSO
     cbkeyfirst, cbkeylast, cbkeynext, cbkeyprev, cbkeysrch.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbkeyalign(cbase_t *cbp, int field)
#else
int cbkeyalign(cbp, field)
cbase_t *cbp;
int field;
#endif
{
	void *		buf	= NULL;
	cbrpos_t	cbrpos	= NIL;
	int		found	= 0;
	lspos_t		lspos	= NIL;

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
	if (lscursor(cbp->lsp) == NULL) {
		/* set key cursor to null */
		if (btsetcur(cbp->btpv[field], NULL) == -1) {
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

	/* get record field */
	if (cbgetrf(cbp, field, buf) == -1) {
		CBEPRINT;
		free(buf);
		return -1;
	}

	/* get record position and place in key */
	if (lsgetcur(cbp->lsp, &lspos) == -1) {
		CBEPRINT;
		free(buf);
		return -1;
	}
	cbrpos = lspos;
	memcpy((char *)buf + cbp->fldv[field].len, &cbrpos, sizeof(cbrpos_t));

	/* position key cursor */
	found = btsearch(cbp->btpv[field], buf);
	if (found == -1) {
		CBEPRINT;
		free(buf);
		return -1;
	}
	if (found == 0) {
		free(buf);
		errno = CBECORRUPT;
		return -1;
	}

	free(buf);
	buf = NULL;

	return 0;
}
