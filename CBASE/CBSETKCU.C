/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbsetkcu.c	1.5 - 91/09/23" */

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
     cbsetkcur - set cbase key cursor

SYNOPSIS
     #include <cbase.h>

     int cbsetkcur(cbp, field, cbkposp)
     cbase_t *cbp;
     int field;
     const cbkpos_t *cbkposp;

DESCRIPTION
     The cbsetkcur function sets the position of a key cursor of the
     specified field in cbase cbp to the value pointed to by cbkposp.
     If cbkposp is the NULL pointer, the key cursor is set to null.
     The record cursor of cbp is positioned to the record associated
     with that key.  Other key cursors are not affected.

     cbsetkcur will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [EINVAL]       field is not a valid field number for cbase cbp.
     [CBELOCK]      cbp is not locked.
     [CBENOPEN]     cbp is not open.

SEE ALSO
     cbgetkcur, cbsetrcur.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbsetkcur(cbase_t *cbp, int field, const cbkpos_t *cbkposp)
#else
int cbsetkcur(cbp, field, cbkposp)
cbase_t *cbp;
int field;
const cbkpos_t *cbkposp;
#endif
{
	void *	buf	= NULL;
	cbrpos_t cbrpos	= NIL;
	lspos_t	lspos	= NIL;
	btpos_t	btpos;

	/* initialize automatic aggregates */
	memset(&btpos, 0, sizeof(btpos));

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

	/* check if not locked */
	if (!(cbp->flags & CBLOCKS)) {
		errno = CBELOCK;
		return -1;
	}

	/* set key cursor position */
	if (cbkposp == NULL) {
		if (btsetcur(cbp->btpv[field], NULL) == -1) {
			CBEPRINT;
			return -1;
		}
	} else {
		memcpy(&btpos, cbkposp, sizeof(btpos));
		if (btsetcur(cbp->btpv[field], &btpos) == -1 ) {
			CBEPRINT;
			return -1;
		}
	}

	/* check if key cursor is null */
	if (btcursor(cbp->btpv[field]) == NULL) {
		if (lssetcur(cbp->lsp, NULL) == -1) {
			CBEPRINT;
			return -1;
		}
		return 0;
	}

	/* get record position */
	buf = calloc((size_t)1, btkeysize(cbp->btpv[field]));
	if (btgetk(cbp->btpv[field], buf) == -1) {
		CBEPRINT;
		free(buf);
		return -1;
	}
	memcpy(&cbrpos, (char *)buf + cbp->fldv[field].len, sizeof(cbrpos));
	free(buf);
	buf = NULL;

	/* set record cursor position */
	lspos = cbrpos;
	if (lspos == NIL) {
		CBEPRINT;
		errno = CBEPANIC;
		return -1;
	}
	if (lssetcur(cbp->lsp, &lspos) == -1) {
		CBEPRINT;
		return -1;
	}

	return 0;
}
