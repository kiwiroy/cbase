/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btlock.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#ifdef AC_STDLIB
#include <stdlib.h>
#endif

/* library headers */
#include <blkio.h>

/* local headers */
#include "btree_.h"

/* function declarations */
#ifdef AC_PROTO
static int resync(btree_t *btp);
#else
static int resync();
#endif

/*man---------------------------------------------------------------------------
NAME
     btlock - btree lock

SYNOPSIS
     #include <btree.h>

     int btlock(btp, ltype)
     btree_t *btp;
     int ltype;

DESCRIPTION
     The btlock function controls the lock status of a btree.  The btp
     argument is an open btree.  ltype indicates the target status of
     the lock on the btree.

     The lock types available are:

          BT_UNLCK - unlock btree
          BT_RDLCK - lock btree for reading
          BT_WRLCK - lock btree for reading and writing
          BT_RDLKW - lock btree for reading (wait)
          BT_WRLKW - lock btree for reading and writing (wait)

     For the lock types which wait, btlock will not return until the
     lock is available.  For the lock types which do not wait, if
     the lock is unavailable because of a lock held by another process
     a value of -1 is returned and errno set to EAGAIN.

     When a btree is unlocked, its cursor is set to null.

     The buffers are flushed before unlocking.

     btlock will fail if one or more of the following is true:

     [EAGAIN]       ltype is BT_RDLCK and btp is already write locked
                    by another process, or ltype is BT_WRLCK and btp
                    is already read or write locked by another
                    process.
     [EINVAL]       btp is is not a valid btree pointer.
     [EINVAL]       ltype is not one of the valid lock types.
     [BTECORRUPT]   btp is corrupt.
     [BTENOPEN]     btp is not open.
     [BTENOPEN]     ltype is BT_RDLCK or BT_RDLKW and btp is not
                    opened for reading or ltype is BT_WRLCK or
                    BT_WRLKW and btp is not open for writing.

SEE ALSO
     btgetlck.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btlock(btree_t *btp, int ltype)
#else
int btlock(btp, ltype)
btree_t *btp;
int ltype;
#endif
{
	int	bltype	= 0;		/* blkio lock type */

	/* validate arguments */
	if (!bt_valid(btp)) {
		errno = EINVAL;
		return -1;
	}

	/* check if btree not open */
	if (!(btp->flags & BTOPEN)) {
		errno = BTENOPEN;
		return -1;
	}

	/* check if btree not open for lock ltype */
	switch (ltype) {
	case BT_UNLCK:
		btp->cbtpos.node = NIL;		/* set cursor to null */
		btp->cbtpos.key = 0;
		bltype = B_UNLCK;
		break;
	case BT_RDLCK:
		if (!(btp->flags & BTREAD)) {
			errno = BTENOPEN;
			return -1;
		}
		bltype = B_RDLCK;
		break;
	case BT_RDLKW:
		if (!(btp->flags & BTREAD)) {
			errno = BTENOPEN;
			return -1;
		}
		bltype = B_RDLKW;
		break;
	case BT_WRLCK:
		if (!(btp->flags & BTWRITE)) {
			errno = BTENOPEN;
			return -1;
		}
		bltype = B_WRLCK;
		break;
	case BT_WRLKW:
		if (!(btp->flags & BTWRITE)) {
			errno = BTENOPEN;
			return -1;
		}
		bltype = B_WRLKW;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	/* lock btree file */
	if (lockb(btp->bp, bltype, (bpos_t)0, (bpos_t)0) == -1) {
		if (errno != EAGAIN) BTEPRINT;
		return -1;
	}

	/* set status bits in btree control structure */
	switch (ltype) {
	case BT_UNLCK:
		btp->flags &= ~BTLOCKS;
		break;
	case BT_RDLCK:
	case BT_RDLKW:
		/* if previously unlocked, re-sync with file */
		if (!(btp->flags & BTLOCKS)) {
			if (resync(btp) == -1) {
				BTEPRINT;
				return -1;
			}
		}
		btp->flags |= BTRDLCK;
		btp->flags &= ~BTWRLCK;
		break;
	case BT_WRLCK:
	case BT_WRLKW:
		/* if previously unlocked, re-sync with file */
		if (!(btp->flags & BTLOCKS)) {
			if (resync(btp) == -1) {
				BTEPRINT;
				return -1;
			}
		}
		btp->flags |= (BTRDLCK | BTWRLCK);
		break;
	default:
		BTEPRINT;
		errno = BTEPANIC;
		return -1;
		break;
	}

	return 0;
}

/* resync:  re-synchronize with file */
#ifdef AC_PROTO
static int resync(btree_t *btp)
#else
static int resync(btp)
btree_t *btp;
#endif
{
	size_t		oldkeysize	= btp->bthdr.keysize;
	unsigned long	oldheight	= 0;
	btpos_t	*	sp		= NULL;
	int		terrno		= 0;		/* tmp errno */

	/* save old tree height */
	oldheight = btp->bthdr.height;

	/* read in header */
	if (bgeth(btp->bp, &btp->bthdr) == -1) {
		BTEPRINT;
		return -1;
	}

	/* check for corruption */
	if (btp->bthdr.flags & BTHMOD) {
		errno = BTECORRUPT;
		return -1;
	}

	/* if first time locked, do setup */
	if (oldkeysize == 0) {
		/* allocate memory for btree */
		if (bt_alloc(btp) == -1) {
			BTEPRINT;
			btp->bthdr.keysize = 0;
			return -1;
		}
		/* set up buffering */
		if (bsetvbuf(btp->bp, NULL, bt_blksize(btp), BTBUFCNT) == -1) {
			BTEPRINT;
			terrno = errno;
			bt_free(btp);
			btp->bthdr.keysize = 0;
			errno = terrno;
			return -1;
		}
	}

	/* adjust for new tree height */
	if (btp->bthdr.height != oldheight) {
		sp = (btpos_t *)realloc(btp->sp, (size_t)(btp->bthdr.height + 1) * sizeof(*sp));
		if (sp == NULL) {
			BTEPRINT;
			errno = ENOMEM;
			return -1;
		}
		btp->sp = sp;
		sp = NULL;
		btp->sp[btp->bthdr.height].node = NIL;
		btp->sp[btp->bthdr.height].key = 0;
	}

	return 0;
}
