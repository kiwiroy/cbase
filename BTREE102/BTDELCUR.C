/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btdelcur.c	1.5 - 91/09/23" */

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
#include <bool.h>

/* local headers */
#include "btree_.h"

/*man---------------------------------------------------------------------------
NAME
     btdelcur - delete current btree key

SYNOPSIS
     #include <btree.h>

     int btdelcur(btp)
     btree_t *btp;

DESCRIPTION
     The btdelcur function deletes the current key from btree btp.
     The cursor is positioned to null.

     btdelcur will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [BTELOCK]      btree btp is not write locked.
     [BTENKEY]      The cursor is null.
     [BTENOPEN]     btree btp is not open.

SEE ALSO
     btdelete, btinsert, btsearch.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btdelcur(btree_t *btp)
#else
int btdelcur(btp)
btree_t *btp;
#endif
{
	int		found	= 0;		/* key found flag */
	void *		key	= NULL;		/* key */

	/* validate arguments */
	if (!bt_valid(btp)) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(btp->flags & BTOPEN)) {
		errno = BTENOPEN;
		return -1;
	}

	/* check lock */
	if (!(btp->flags & BTWRLCK)) {
		errno = BTELOCK;
		return -1;
	}

	/* check if cursor is null */
	if (btcursor(btp) == NULL) {
		errno = BTENKEY;
		return -1;
	}

	/* generate search path if necessary */
	if (btp->cbtnp->n < bt_ndmin(btp) + 1) {
		key = calloc((size_t)1, btp->bthdr.keysize);
		if (key == NULL) {
			BTEPRINT;
			return -1;
		}
		memcpy(key, bt_kykeyp(btp, btp->cbtnp, btp->cbtpos.key), btp->bthdr.keysize);
		found = btsearch(btp, key);
		if (found == -1) {
			BTEPRINT;
			free(key);
			return -1;
		}
		free(key);
		key = NULL;
		if (found == 0) {
			BTEPRINT;
			errno = BTEPANIC;
			return -1;
		}
	}

	/* delete the current key */
	if (bt_delete(btp) == -1) {
		BTEPRINT;
		return -1;
	}

	return 0;
}
