/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btgetlck.c	1.5 - 91/09/23" */

#include <ansi.h>

/* local headers */
#include "btree_.h"

/*man---------------------------------------------------------------------------
NAME
     btgetlck - get btree lock status

SYNOPSIS
     #include <btree.h>

     int btgetlck(btp)
     btree_t *btp;

DESCRIPTION
     The btgetlck function reports the lock status of a btree.  The
     btp argument is an open btree.  The function returns the status
     of the lock currently held by the calling process.  Locks held by
     other processes are not reported.

     The possible return values are:

          BT_UNLCK     btree not locked
          BT_RDLCK     btree locked for reading
          BT_WRLCK     btree locked for reading and writing

SEE ALSO
     btlock.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btgetlck(btree_t *btp)
#else
int btgetlck(btp)
btree_t *btp;
#endif
{
	if (!(btp->flags & BTLOCKS)) {
		return BT_UNLCK;
	} else if (btp->flags & BTWRLCK) {
		return BT_WRLCK;
	}

	return BT_RDLCK;
}
