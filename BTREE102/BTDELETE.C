/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btdelete.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* library headers */
#include <blkio.h>

/* local headers */
#include "btree_.h"

/*man---------------------------------------------------------------------------
NAME
     btdelete - delete btree key

SYNOPSIS
     #include <btree.h>

     int btdelete(btp, buf)
     btree_t *btp;
     const void *buf;

DESCRIPTION
     The btdelete function searches for the key in btree btp with the
     value pointed to by buf and deletes it.  The cursor is positioned
     to null.

     btdelete will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       buf is the NULL pointer.
     [BTELOCK]      btp is not write locked.
     [BTENKEY]      The key pointed to by buf is not in btp.
     [BTENOPEN]     btp is not open.

SEE ALSO
     btdelcur, btinsert, btsearch.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btdelete(btree_t *btp, const void *buf)
#else
int btdelete(btp, buf)
btree_t *btp;
const void *buf;
#endif
{
	int found = 0;

	/* validate arguments */
	if (!bt_valid(btp) || buf == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(btp->flags & BTOPEN)) {
		errno = BTENOPEN;
		return -1;
	}

	/* check if not write locked */
	if (!(btp->flags & BTWRLCK)) {
		errno = BTELOCK;
		return -1;
	}

	/* make buf the current key and generate search path */
	found = btsearch(btp, buf);
	if (found == -1) {
		BTEPRINT;
		return -1;
	}
	if (found == 0) {		/* key not in btree btp */
		errno = BTENKEY;
		return -1;
	}

	/* delete the current key */
	if (bt_delete(btp) == -1) {
		BTEPRINT;
		return -1;
	}

	return 0;
}
