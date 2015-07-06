/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbgetlck.c	1.5 - 91/09/23" */

#include <ansi.h>

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cbgetlck - get cbase lock status

SYNOPSIS
     #include <cbase.h>

     int cbgetlck(cbp)
     cbase_t *cbp;

DESCRIPTION
     The cbgetlck function reports the lock status of cbase cbp.  The
     function returns the status of the lock currently held by the
     calling process.  Locks held by other processes are not reported.

     The possible return values are:

     CB_UNLCK       cbase not locked
     CB_RDLCK       cbase locked for reading
     CB_WRLCK       cbase locked for reading and writing

SEE ALSO
     cblock.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbgetlck(cbase_t *cbp)
#else
int cbgetlck(cbp)
cbase_t *cbp;
#endif
{
	if (!(cbp->flags & CBLOCKS)) {
		return CB_UNLCK;
	} else if (cbp->flags & CBWRLCK) {
		return CB_WRLCK;
	}

	return CB_RDLCK;
}
