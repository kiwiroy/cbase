/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lsdelcur.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* library headers */
#include <blkio.h>

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     lsdelcur - delete current lseq record

SYNOPSIS
     #include <lseq.h>

     int lsdelcur(lsp)
     lseq_t *lsp;

DESCRIPTION
     The lsdelcur function deletes the current record from lseq lsp.
     The The cursor is positioned to the record following the deleted
     record.

     lsdelcur will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [LSELOCK]      lsp is not write locked.
     [LSENOPEN]     lsp is not open.
     [LSENREC]      The cursor is null.

SEE ALSO
     lsinsert, lssearch.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lsdelcur(lseq_t * lsp)
#else
int lsdelcur(lsp)
lseq_t * lsp;
#endif
{
	bpos_t bpos = 0;

	/* validate arguments */
	if (!ls_valid(lsp)) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(lsp->flags & LSOPEN)) {
		errno = LSENOPEN;
		return -1;
	}

	/* check if not write locked */
	if (!(lsp->flags & LSWRLCK)) {
		errno = LSELOCK;
		return -1;
	}

	/* check if cursor is null */
	if (lsp->clspos == NIL) {
		errno = LSENREC;
		return -1;
	}

	/* set modify bit in header */
	lsp->lshdr.flags |= LSHMOD;
	if (bputhf(lsp->bp, sizeof(bpos_t),
		(void *)((char *)&lsp->lshdr + sizeof(bpos_t)),
			sizeof(lshdr_t) - sizeof(bpos_t)) == -1) {
		LSEPRINT;
		return -1;
	}
	if (bsync(lsp->bp) == -1) {
		LSEPRINT;
		return -1;
	}

	/* fix links */
	if (lsp->clsrp->next == NIL) {
		lsp->lshdr.last = lsp->clsrp->prev;
	} else {
		if (bputbf(lsp->bp, (bpos_t)lsp->clsrp->next, offsetof(lsrec_t, prev), &lsp->clsrp->prev, sizeof(lsp->clsrp->prev)) == -1) {
			LSEPRINT;
			return -1;
		}
	}
	if (lsp->clsrp->prev == NIL) {
		lsp->lshdr.first = lsp->clsrp->next;
	} else {
		if (bputbf(lsp->bp, (bpos_t)lsp->clsrp->prev, offsetof(lsrec_t, next), &lsp->clsrp->next, sizeof(lsp->clsrp->next)) == -1) {
			LSEPRINT;
			return -1;
		}
	}

	/* decrement record count */
	--lsp->lshdr.reccnt;

	/* return block to free list */
	bpos = lsp->clspos;
	if (bflpush(lsp->bp, &bpos) == -1) {
		LSEPRINT;
		return -1;
	}

	/* clear modify bit in header */
	lsp->lshdr.flags &= ~LSHMOD;
	if (bputhf(lsp->bp, sizeof(bpos_t),
		(void *)((char *)&lsp->lshdr + sizeof(bpos_t)),
			sizeof(lshdr_t) - sizeof(bpos_t)) == -1) {
		LSEPRINT;
		return -1;
	}
	if (bsync(lsp->bp) == -1) {
		LSEPRINT;
		return -1;
	}

	/* position cursor to next record */
	if (lsnext(lsp) == -1) {
		LSEPRINT;
		return -1;
	}

	return 0;
}
