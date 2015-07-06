/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btsetbuf.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>

/* library headers */
#include <blkio.h>

/* local headers */
#include "btree_.h"

/*man---------------------------------------------------------------------------
NAME
     btsetbuf - assign buffering to a btree

SYNOPSIS
     #include <btree.h>

     int btsetbuf(btp, buf)
     btree_t *btp;
     void *buf;

DESCRIPTION
     The btsetbuf function causes the storage area pointed to by buf
     to be used by the btree associated with btree pointer btp instead
     of an automatically allocated buffer area.  If buf is the NULL
     pointer, btp will be completely unbuffered.

     The size of the storage area needed can be obtained using the
     BTBUFSIZE() macro:

          char buf[BTBUFSIZE(M, KEYSIZE, BTBUFCNT)];
          btsetbuf(btp, buf);

     where M is the degree of the btree, KEYSIZE is the size of the
     keys int the btree, and BTBUFCNT is the default number of nodes
     buffered when a btree is opened.  BTBUFCNT is defined in
     <btree.h>.  If the number of nodes buffered has been changed using
     btsetvbuf, then that number should be used in place of BTBUFCNT.

     btsetbuf may be called at any time after opening the btree,
     before and after it is read or written; the buffers are flushed
     before installing the new buffer area.

     btsetbuf will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [BTENBUF]      buf is not the NULL pointer and btp
                    is not buffered.
     [BTENOPEN]     btp is not open.

SEE ALSO
     btsetvbuf, btsync.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btsetbuf(btree_t *btp, void *buf)
#else
int btsetbuf(btp, buf)
btree_t *btp;
void *buf;
#endif
{
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

	/* set buffering */
	if (bsetbuf(btp->bp, buf) == -1) {
		if (errno == BENBUF) errno = BTENBUF;
		BTEPRINT;
		return -1;
	}

	return 0;
}
