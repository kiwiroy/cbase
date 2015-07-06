/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btsetvbu.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif

/* library headers */
#include <blkio.h>

/* local headers */
#include "btree_.h"

/*man---------------------------------------------------------------------------
NAME
     btsetvbuf - assign buffering to a btree

SYNOPSIS
     #include <btree.h>

     int btsetvbuf(btp, buf, bufcnt)
     btree_t *btp;
     void *buf;
     size_t bufcnt;

DESCRIPTION
     The btsetvbuf function is used to assign buffering to a btree.
     bufcnt specifies the number of btree nodes to be buffered.  If
     bufcnt has a value of zero, the btree will be completely
     unbuffered.  If buf is not the NULL pointer, the storage area it
     points to will be used instead of one automatically allocated for
     buffering.

     The size of the storage area needed can be obtained using the
     BTBUFSIZE() macro:

          char buf[BTBUFSIZE(M, KEYSIZE, BUFCNT)];
          btsetvbuf(btp, buf, BUFCNT);

     where M is the degree of the btree, KEYSIZE is the size of the
     keys int the btree, and BUFCNT is the number of nodes to buffer.

     Any previously buffered data is flushed before installing the new
     buffer area, so btsetvbuf may be called more than once.  This
     allows the buffer size to be varied with the file size.

     btsetvbuf will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree.
     [ENOMEM]       Not enough memory is available for the calling
                    process to allocate.
     [BTENOPEN]     btp is not open.

SEE ALSO
     btsetbuf, btsync.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btsetvbuf(btree_t *btp, void *buf, size_t bufcnt)
#else
int btsetvbuf(btp, buf, bufcnt)
btree_t *btp;
void *buf;
size_t bufcnt;
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
	if (bsetvbuf(btp->bp, buf, bt_blksize(btp), bufcnt) == -1) {
		BTEPRINT;
		return -1;
	}

	return 0;
}
