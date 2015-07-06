/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbkeysrc.c	1.5 - 91/09/23" */

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

/* non-ansi headers */
#include <bool.h>

/* library headers */
#include <blkio.h>
#include <btree.h>
#include <lseq.h>

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cbkeysrch - search cbase key

SYNOPSIS
     #include <cbase.h>

     int cbkeysrch(cbp, field, buf)
     cbase_t *cbp;
     int field;
     const void *buf;

DESCRIPTION
     The cbkeysrch function searches a key in cbase cbp.  field is the
     field to be searched.  buf points to the key for which to be
     searched.  If found, the cursor for that key is positioned to the
     found key.  If it is not found, the cursor for that key is
     positioned to the next higher key, which may be null.  The record
     cursor is set to the record associated with the key marked by the
     key cursor.  Other key cursors are not affected.

     cbkeysrch will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [EINVAL]       field is not a valid field number for
                    cbase cbp.
     [EINVAL]       buf is the NULL pointer.
     [CBELOCK]      cbp is not read locked.
     [CBENKEY]      field is not a key.
     [CBENOPEN]     cbp is not open.

SEE ALSO
     cbkcursor, cbkeyfirst, cbkeylast, cbkeynext, cbkeyprev.

DIAGNOSTICS
     Upon successful completion, a value of 1 is returned if the key
     was found or a value of 0 if it was not.  Otherwise, a value of
     -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbkeysrch(cbase_t *cbp, int field, const void *buf)
#else
int cbkeysrch(cbp, field, buf)
cbase_t *cbp;
int field;
const void *buf;
#endif
{
	void *	buf2	= NULL;
	cbrpos_t cbrpos	= NIL;
	lspos_t	lspos	= NIL;
	bool	found	= FALSE;

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

	/* validate arguments */
	if (field < 0 || field >= cbp->fldc) {
		errno = EINVAL;
		return -1;
	}

	/* check if field is a key */
	if (!(cbp->fldv[field].flags & CB_FKEY)) {
		errno = CBENKEY;
		return -1;
	}

	/* check if not read locked */
	if (!(cbp->flags & CBRDLCK)) {
		errno = CBELOCK;
		return -1;
	}

	/* construct (key, record position) pair */
	if (btkeysize(cbp->btpv[field]) != (cbp->fldv[field].len + sizeof(cbrpos_t))) {
		CBEPRINT;
		errno = CBEPANIC;
		return -1;
	}
	buf2 = calloc((size_t)1, cbp->fldv[field].len + sizeof(cbrpos_t));
	if (buf2 == NULL) {
		CBEPRINT;
		errno = ENOMEM;
		return -1;
	}
	cbrpos = NIL;
	memcpy(buf2, buf, cbp->fldv[field].len);
	memcpy((char *)buf2 + cbp->fldv[field].len, &cbrpos, sizeof(cbrpos_t));

	/* search for key */
	if (btsearch(cbp->btpv[field], buf2) == -1) {
		CBEPRINT;
		free(buf2);
		return -1;
	}

	/* check if cursor is null */
	if (btcursor(cbp->btpv[field]) == NULL) {
		free(buf2);
		if (lssetcur(cbp->lsp, NULL) == -1) {
			CBEPRINT;
			return -1;
		}
		return 0;
	}

	/* get record position */
	if (btgetk(cbp->btpv[field], buf2) == -1) {
		CBEPRINT;
		free(buf2);
		return -1;
	}
	memcpy(&cbrpos, (char *)buf2 + cbp->fldv[field].len, sizeof(cbrpos));

	/* check if key found or not */
	if ((*cbcmpv[cbp->fldv[field].type])(buf, buf2, cbp->fldv[field].len) == 0) {
		found = TRUE;
	} else {
		found = FALSE;
	}
	free(buf2);
	buf2 = NULL;

	/* set record cursor */
	lspos = cbrpos;
	if (lspos == NIL) {
		CBEPRINT;
		errno = CBEPANIC;
	}
	if (lssetcur(cbp->lsp, &lspos) == -1) {
		CBEPRINT;
		return -1;
	}

	return found ? 1 : 0;
}
