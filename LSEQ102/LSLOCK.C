/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lslock.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* library headers */
#include <blkio.h>

/* local headers */
#include "lseq_.h"

/* function declarations */
#ifdef AC_PROTO
static int resync(lseq_t *lsp);
#else
static int resync();
#endif

/*man---------------------------------------------------------------------------
NAME
     lslock - lseq lock

SYNOPSIS
     #include <lseq.h>

     int lslock(lsp, ltype)
     lseq_t *lsp;
     int ltype;

DESCRIPTION
     The lslock function controls the lock status of an lseq.  The lsp
     argument is an open lseq.  ltype indicates the target status of
     the lock on the lseq.

     The lock types available are:

          LS_UNLCK - unlock lseq
          LS_RDLCK - lock lseq for reading
          LS_WRLCK - lock lseq for reading and writing
          LS_RDLKW - lock lseq for reading (wait)
          LS_WRLKW - lock lseq for reading and writing (wait)

     For the lock types which wait, lslock will not return until the
     lock is available.  For the lock types which do not wait, if the
     lock is unavailable because of a lock held by another process  a
     value of -1 is returned and errno set to EAGAIN.

     When an lseq is unlocked, its cursor is set to null.

     lslock will fail if one or more of the following is true:

     [EAGAIN]       ltype is LS_RDLCK and lsp is already
                    write locked by another process, or
                    ltype is LS_WRLCK and lsp is already
                    read or write locked by another process.
     [EINVAL]       lsp is is not a valid lseq pointer.
     [EINVAL]       ltype is not one of the valid lock
                    types.
     [LSECORRUPT]   lsp is corrupt.
     [LSENOPEN]     lsp is not open.
     [LSENOPEN]     ltype is LS_RDLCK or LS_RDLKW and lsp
                    is not opened for reading or ltype is
                    LS_WRLCK or LS_WRLKW and lsp is not open
                    for writing.

SEE ALSO
     lsgetlck.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lslock(lseq_t *lsp, int ltype)
#else
int lslock(lsp, ltype)
lseq_t *lsp;
int ltype;
#endif
{
	int	bltype	= 0;	/* blkio lock type */

	/* validate arguments */
	if (!ls_valid(lsp)) {
		errno = EINVAL;
		return -1;
	}

	/* check if lseq not open */
	if (!(lsp->flags & LSOPEN)) {
		errno = LSENOPEN;
		return -1;
	}

	/* check if lseq not open for lock ltype */
	switch (ltype) {
	case LS_UNLCK:
		lsp->clspos = NIL;	/* set cursor to null */
		bltype = B_UNLCK;
		break;
	case LS_RDLCK:
		if (!(lsp->flags & LSREAD)) {
			errno = LSENOPEN;
			return -1;
		}
		bltype = B_RDLCK;
		break;
	case LS_RDLKW:
		if (!(lsp->flags & LSREAD)) {
			errno = LSENOPEN;
			return -1;
		}
		bltype = B_RDLKW;
		break;
	case LS_WRLCK:
		if (!(lsp->flags & LSWRITE)) {
			errno = LSENOPEN;
			return -1;
		}
		bltype = B_WRLCK;
		break;
	case LS_WRLKW:
		if (!(lsp->flags & LSWRITE)) {
			errno = LSENOPEN;
			return -1;
		}
		bltype = B_WRLKW;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	/* lock lseq file */
	if (lockb(lsp->bp, bltype, (bpos_t)0, (bpos_t)0) == -1) {
		if (errno != EAGAIN) LSEPRINT;
		return -1;
	}

	/* set status bits in lseq control structure */
	switch (ltype) {
	case LS_UNLCK:
		lsp->flags &= ~LSLOCKS;
		break;
	case LS_RDLCK:
	case LS_RDLKW:
		/* if previously unlocked, re-sync with file */
		if (!(lsp->flags & LSLOCKS)) {
			if (resync(lsp) == -1) {
				LSEPRINT;
				return -1;
			}
		}
		lsp->flags |= LSRDLCK;
		lsp->flags &= ~LSWRLCK;
		break;
	case LS_WRLCK:
	case LS_WRLKW:
		/* if previously unlocked, re-sync with file */
		if (!(lsp->flags & LSLOCKS)) {
			if (resync(lsp) == -1) {
				LSEPRINT;
				return -1;
			}
		}
		lsp->flags |= (LSRDLCK | LSWRLCK);
		break;
	default:
		LSEPRINT;
		errno = LSEPANIC;
		return -1;
		break;
	}

	return 0;
}

/* resync:  re-synchronize with file */
#ifdef AC_PROTO
static int resync(lseq_t *lsp)
#else
static int resync(lsp)
lseq_t *lsp;
#endif
{
	size_t	oldrecsize	= lsp->lshdr.recsize;	/* record size */
	int	terrno		= 0;			/* tmp errno */

	/* read in header */
	if (bgeth(lsp->bp, &lsp->lshdr) == -1) {
		LSEPRINT;
		return -1;
	}

	/* check for corruption */
	if (lsp->lshdr.flags & LSHMOD) {
		errno = LSECORRUPT;
		return -1;
	}

	/* if first time locked, do setup */
	if (oldrecsize == 0) {
		/* allocate memory for lseq */
		if (ls_alloc(lsp) == -1) {
			LSEPRINT;
			lsp->lshdr.recsize = 0;
			return -1;
		}
		/* set up buffering */
		if (bsetvbuf(lsp->bp, NULL, ls_blksize(lsp), LSBUFCNT) == -1) {
			LSEPRINT;
			terrno = errno;
			ls_free(lsp);
			lsp->lshdr.recsize = 0;
			errno = terrno;
			return -1;
		}
	}

	return 0;
}
