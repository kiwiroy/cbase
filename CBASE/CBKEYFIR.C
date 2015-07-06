/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbkeyfir.c	1.5 - 91/09/23" */

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
     cbkeyfirst - first cbase key

SYNOPSIS
     #include <cbase.h>

     int cbkeyfirst(cbp, field)
     cbase_t *cbp;
     int field;

DESCRIPTION
     The cbkeyfirst function positions the key cursor of the specified
     field in cbase cbp to the first key.  The record cursor of cbp is
     positioned to the record associated with that key.  Other key
     cursors are not affected.

     cbkeyfirst will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [EINVAL]       field is not a valid field number for
                    cbase cbp.
     [CBELOCK]      cbp is not locked.
     [CBENKEY]      field is not a key.
     [CBENOPEN]     cbp is not open.
     [CBENREC]      cbp is empty.

SEE ALSO
     cbkeylast, cbkeynext, cbkeyprev, cbreccnt, cbrecfirst.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbkeyfirst(cbase_t *cbp, int field)
#else
int cbkeyfirst(cbp, field)
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

	/* check if cbase empty */
	if (lsreccnt(cbp->lsp) == 0) {
		errno = CBENREC;
		return -1;
	}

	/* set key cursor to first key */
	if (btfirst(cbp->btpv[field]) == -1) {
		CBEPRINT;
		return -1;
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