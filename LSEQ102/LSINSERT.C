/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lsinsert.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <blkio.h>

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     lsinsert - insert record

SYNOPSIS
     #include <lseq.h>

     int lsinsert(lsp, buf)
     lseq_t *lsp;
     const void *buf;

DESCRIPTION
     The lsinsert function creates a new record following the current
     record and writes the data pointed to by buf into that record.
     The cursor is set to the inserted record.

     lsinsert will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [EINVAL]       buf is the NULL pointer.
     [LSELOCK]      lsp is not write locked.
     [LSENOPEN]     lsp is not open.

SEE ALSO
     lsdelcur, lsinsert, lssearch.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lsinsert(lseq_t *lsp, const void *buf)
#else
int lsinsert(lsp, buf)
lseq_t *lsp;
const void *buf;
#endif
{
	int	terrno	= 0;
	bpos_t	bpos	= NIL;

	/* validate arguments */
	if (!ls_valid(lsp) || buf == NULL) {
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

	/* build record to insert */
	lsp->clsrp->prev = lsp->clspos;
	if (lsp->clspos == NIL) {
		lsp->clsrp->next = lsp->lshdr.first;
	}
	memcpy(lsp->clsrp->recbuf, buf, lsp->lshdr.recsize);

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

	/* get empty block from free list */
	if (bflpop(lsp->bp, &bpos) == -1) {
		LSEPRINT;
		return -1;
	}
	lsp->clspos = bpos;

	/* write new record */
	if (ls_rcput(lsp, lsp->clspos, lsp->clsrp) == -1) {
		LSEPRINT;
		terrno = errno;
		bpos = lsp->clspos;
		bflpush(lsp->bp, &bpos);
		errno = terrno;
		return -1;
	}

	/* increment record count */
	lsp->lshdr.reccnt++;

	/* fix links */
	if (lsp->clsrp->next == NIL) {
		lsp->lshdr.last = lsp->clspos;
	} else {
		if (bputbf(lsp->bp, (bpos_t)lsp->clsrp->next, offsetof(lsrec_t, prev), &lsp->clspos, sizeof(lsp->clsrp->prev)) == -1) {
			LSEPRINT;
			terrno = errno;
			bpos = lsp->clspos;
			bflpush(lsp->bp, &bpos);
			errno = terrno;
			return -1;
		}
	}
	if (lsp->clsrp->prev == NIL) {
		lsp->lshdr.first = lsp->clspos;
	} else {
		if (bputbf(lsp->bp, (bpos_t)lsp->clsrp->prev, offsetof(lsrec_t, next), &lsp->clspos, sizeof(lsp->clsrp->next)) == -1) {
			LSEPRINT;
			terrno = errno;
			bpos = lsp->clspos;
			bflpush(lsp->bp, &bpos);
			errno = terrno;
			return -1;
		}
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

	return 0;
}
