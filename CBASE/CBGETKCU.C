/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbgetkcu.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <blkio.h>
#include <btree.h>

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cbgetkcur - get cbase key cursor

SYNOPSIS
     #include <cbase.h>

     int cbgetkcur(cbp, field, cbkposp)
     cbase_t *cbp;
     int field;
     cbkpos_t *cbkposp;

DESCRIPTION
     The cbgetkcur function gets the position of the key cursor of the
     specified field in cbase cbp.  The cursor position is returned in
     the location pointed to by cbkposp.

     cbgetkcur will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [EINVAL]       field is not a valid field number for cbase cbp.
     [EINVAL]       cbkposp is the NULL pointer.
     [CBELOCK]      cbp is not locked.
     [CBENOPEN]     cbp is not open.

SEE ALSO
     cbgetrcur, cbsetkcur.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbgetkcur(cbase_t *cbp, int field, cbkpos_t *cbkposp)
#else
int cbgetkcur(cbp, field, cbkposp)
cbase_t *cbp;
int field;
cbkpos_t *cbkposp;
#endif
{
	btpos_t btpos;

	/* initialize automatic aggregates */
	memset(&btpos, 0, sizeof(btpos));

	/* validate arguments */
	if (!cb_valid(cbp) || cbkposp == NULL) {
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

	/* get key cursor position */
	if (btgetcur(cbp->btpv[field], &btpos) == -1) {
		CBEPRINT;
		return -1;
	}

	/* load return argument */
	memcpy(cbkposp, &btpos, sizeof(cbkpos_t));

	return 0;
}
