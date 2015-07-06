/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cblock.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* library headers */
#include <btree.h>
#include <lseq.h>

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cblock - lock cbase

SYNOPSIS
     #include <cbase.h>

     int cblock(cbp, ltype)
     cbase_t *cbp;
     int ltype;

DESCRIPTION
     The cblock function controls the lock status of a cbase.  The cbp
     argument is an open cbase.  ltype indicates the target status of
     the lock on the cbase.

     The lock types available are:

     CB_UNLCK       unlock cbase
     CB_RDLCK       lock cbase for reading
     CB_WRLCK       lock cbase for reading and writing
     CB_RDLKW       lock cbase for reading (wait)
     CB_WRLKW       lock cbase for reading and writing (wait)

     For the lock types which wait, cblock will not return until the
     lock is available.  For the lock types which do not wait, if the
     lock is unavailable because of a lock held by another process  a
     value of -1 is returned and errno set to EAGAIN.

     cblock will fail if one or more of the following is true:

     [EAGAIN]       ltype is CB_RDLCK and the cbase is
                    already write locked by another process,
                    or ltype is CB_WRLCK and the cbase is
                    already read or write locked by another
                    process.
     [EINVAL]       cbp is is not a valid cbase.
     [EINVAL]       ltype is not a valid lock type.
     [CBECORRUPT]   Either the record file or one of the index
                    files of cbp is corrupt.
     [CBENOPEN]     cbp is not open.
     [CBENOPEN]     ltype is CB_RDLCK or CB_RDLKW and cbp
                    is not opened for reading or ltype is
                    CB_WRLCK or CB_WRLKW and cbp not open
                    for writing.

SEE ALSO
     cbgetlck.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cblock(cbase_t *cbp, int ltype)
#else
int cblock(cbp, ltype)
cbase_t *cbp;
int ltype;
#endif
{
	int	btltype	= 0;		/* btree lock type */
	int	i	= 0;		/* loop counter */
	int	lsltype	= 0;		/* lseq lock type */

	/* validate arguments */
	if (!cb_valid(cbp)) {
		errno = EINVAL;
		return -1;
	}

	/* check if cbase not open */
	if (!(cbp->flags & CBOPEN)) {
		errno = CBENOPEN;
		return -1;
	}

	/* check open flags and set lsltype and btltype */
	switch (ltype) {
	case CB_RDLCK:
		if (!(cbp->flags & CBREAD)) {
			errno = CBENOPEN;
			return -1;
		}
		lsltype = LS_RDLCK;
		btltype = BT_RDLCK;
		break;
	case CB_RDLKW:
		if (!(cbp->flags & CBREAD)) {
			errno = CBENOPEN;
			return -1;
		}
		lsltype = LS_RDLKW;
		btltype = BT_RDLKW;
		break;
	case CB_WRLCK:
		if (!(cbp->flags & CBWRITE)) {
			errno = CBENOPEN;
			return -1;
		}
		lsltype = LS_WRLCK;
		btltype = BT_WRLCK;
		break;
	case CB_WRLKW:
		if (!(cbp->flags & CBWRITE)) {
			errno = CBENOPEN;
			return -1;
		}
		lsltype = LS_WRLKW;
		btltype = BT_WRLKW;
		break;
	case CB_UNLCK:
		lsltype = LS_UNLCK;
		btltype = BT_UNLCK;
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	/* unlock in reverse order of locking */
	if (ltype == CB_UNLCK) {
		/* unlock index files */
		for (i = cbp->fldc - 1; i >= 0; --i) {
			if (cbp->fldv[i].flags & CB_FKEY) {
				if (btlock(cbp->btpv[i], btltype) == -1) {
					CBEPRINT;
					return -1;
				}
			}
		}
	}

	/* lock data file */
	if (lslock(cbp->lsp, lsltype) == -1) {
		if (errno == EAGAIN) {
			return -1;
		}
		if (errno == LSECORRUPT) {
			errno = CBECORRUPT;
			return -1;
		}
		CBEPRINT;
		return -1;
	}

	if (ltype != CB_UNLCK) {
		/* lock index files */
		for (i = 0; i < cbp->fldc; ++i) {
			if (cbp->fldv[i].flags & CB_FKEY) {
				if (btlock(cbp->btpv[i], btltype) == -1) {
					CBEPRINT;
					return -1;
				}
			}
		}
	}

	/* set lock bits */
	switch (ltype) {
	case CB_RDLCK:
	case CB_RDLKW:
		cbp->flags |= CBRDLCK;
		cbp->flags &= ~CBWRLCK;
		break;
	case CB_WRLCK:
	case CB_WRLKW:
		cbp->flags |= (CBRDLCK | CBWRLCK);
		break;
	case CB_UNLCK:
		cbp->flags &= ~CBLOCKS;
		break;
	default:
		CBEPRINT;
		errno = CBEPANIC;
		return -1;
		break;
	}

	return 0;
}
