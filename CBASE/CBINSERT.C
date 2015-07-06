/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbinsert.c	1.5 - 91/09/23" */

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

/* library headers */
#include <blkio.h>
#include <lseq.h>

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cbinsert - insert record

SYNOPSIS
     #include <cbase.h>

     int cbinsert(cbp, buf)
     cbase_t *cbp;
     const void *buf;

DESCRIPTION
     The cbinsert function inserts the record pointed to by buf into
     cbase cbp.  The record is inserted after the current record.  The
     record cursor and all the key cursors are set to the inserted
     record.

     cbinsert will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [EINVAL]       buf is the NULL pointer.
     [CBEDUP]       A field in the record pointed to by buf contains
                    an illegal duplicate key.
     [CBELOCK]      cbp is not write locked.
     [CBENOPEN]     cbp is not open.

SEE ALSO
     cbdelcur, cbrcursor.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbinsert(cbase_t *cbp, const void *buf)
#else
int cbinsert(cbp, buf)
cbase_t *cbp;
const void *buf;
#endif
{
	void *		buf2	= NULL;
	cbrpos_t	cbrpos	= NIL;
	int		found	= 0;
	int		i	= 0;
	lspos_t		lspos	= NIL;

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

	/* check if not write locked */
	if (!(cbp->flags & CBWRLCK)) {
		errno = CBELOCK;
		return -1;
	}

	/* check for illegal duplicate keys */
	if (cbgetrcur(cbp, &cbrpos) == -1) {	/* save current record pos */
		CBEPRINT;
		return -1;
	}
	for (i = 0; i < cbp->fldc; ++i) {
		if (cbp->fldv[i].flags & CB_FKEY) {
			if (cbp->fldv[i].flags & CB_FUNIQ) {
				buf2 = calloc((size_t)1, cbp->fldv[i].len);
				if (buf2 == NULL) {
					CBEPRINT;
					errno = ENOMEM;
					return -1;
				}
				memcpy(buf2, (char *)buf + cbp->fldv[i].offset, cbp->fldv[i].len);
				found = cbkeysrch(cbp, i, buf2);
				if (found == -1) {
					CBEPRINT;
					free(buf2);
					return -1;
				}
				if (found == 1) {
					free(buf2);
					errno = CBEDUP;
					return -1;
				}
				free(buf2);
				buf2 = NULL;
			}
		}
	}
	if (cbsetrcur(cbp, &cbrpos) == -1) {	/* restore record pos */
		CBEPRINT;
		return -1;
	}

	/* insert record */
	if (lsinsert(cbp->lsp, buf) == -1) {
		CBEPRINT;
		return -1;
	}

	/* get position of inserted record */
	if (lsgetcur(cbp->lsp, &lspos) == -1) {
		CBEPRINT;
		return -1;
	}
	cbrpos = lspos;

	/* insert keys */
	for (i = 0; i < cbp->fldc; ++i) {
		if (cbp->fldv[i].flags & CB_FKEY) {
			/* construct (key, record position) pair */
			if (btkeysize(cbp->btpv[i]) != (cbp->fldv[i].len + sizeof(cbrpos_t))) {
				CBEPRINT;
				errno = CBEPANIC;
				return -1;
			}
			buf2 = calloc((size_t)1, cbp->fldv[i].len + sizeof(cbrpos_t));
			if (buf2 == NULL) {
				CBEPRINT;
				errno = ENOMEM;
				return -1;
			}
			memcpy(buf2, (char *)buf + cbp->fldv[i].offset, cbp->fldv[i].len);
			memcpy((char *)buf2 + cbp->fldv[i].len, &cbrpos, sizeof(cbrpos_t));
			/* insert key */
			if (btinsert(cbp->btpv[i], buf2) == -1) {
				CBEPRINT;
				free(buf2);
				return -1;
			}
			free(buf2);
			buf2 = NULL;
		}
	}

	return 0;
}
