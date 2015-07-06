/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)bsetvbuf.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif

/* local headers */
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bsetvbuf - assign buffering to a block file

SYNOPSIS
     #include <blkio.h>

     int bsetvbuf(bp, buf, blksize, bufcnt)
     BLKFILE *bp;
     void *buf;
     size_t blksize;
     size_t bufcnt;

DESCRIPTION
     The bsetvbuf function causes the block size and buffer block
     count of the block file associated with BLKFILE pointer bp to be
     changed to bufcnt and blksize, with the same effects as if the
     file had been opened with these values.  If bufcnt has a value of
     zero, the file will be completely unbuffered.  If buf is not the
     NULL pointer, the storage area it points to will be used instead
     of one automatically allocated for buffering.  In this case, buf
     must point to a storage area of size no less than

          header size + block size * buffer count

     bsetvbuf may be called at any time after opening the block file,
     before and after it is read or written.

     bsetvbuf will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       blksize is less than 1.
     [ENOMEM]       Enough memory is not available for the
                    calling process to allocate.
     [BENOPEN]      bp is not open.

SEE ALSO
     bopen, bsetbuf.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

NOTES
     A common source of error is allocating buffer space as an
     automatic variable in a code block, and then failing to close the
     block file in the same block.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bsetvbuf(BLKFILE *bp, void *buf, size_t blksize, size_t bufcnt)
#else
int bsetvbuf(bp, buf, blksize, bufcnt)
BLKFILE *bp;
void *buf;
size_t blksize;
size_t bufcnt;
#endif
{
	/* validate arguments */
	if (!b_valid(bp) || blksize == 0) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOOPEN)) {
		errno = BENOPEN;
		return -1;
	}

	/* synchronize file with buffers */
	if (bsync(bp) == -1) {
		BEPRINT;
		return -1;
	}

	/* free old buffer storage */
	b_free(bp);

	/* modify BLKFILE control structure contents */
	bp->flags &= ~BIOUSRBUF;
	bp->blksize = blksize;
	bp->bufcnt = bufcnt;
	bp->endblk = 0;
	bp->most = 0;
	bp->least = 0;
	if (b_uendblk(bp, &bp->endblk) == -1) {
		BEPRINT;
		return -1;
	}

	/* check if not buffered */
	if (bp->bufcnt == 0) {
		return 0;
	}

	/* set user supplied buffer flag */
	if (buf != NULL) {
		bp->flags |= BIOUSRBUF;
	}

	/* allocate memory for bp */
	if (b_alloc(bp) == -1) {
		BEPRINT;
		return -1;
	}

	/* connect user supplied buffer */
	if (bp->flags & BIOUSRBUF) {
		bp->blkbuf = buf;
	}

	/* initialize linked list of buffers */
	if (b_initlist(bp) == -1) {
		BEPRINT;
		return -1;
	}

	return 0;
}
