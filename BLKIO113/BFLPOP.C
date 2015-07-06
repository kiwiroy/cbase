/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)bflpop.c	1.5 - 91/09/23" */

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
     bflpop - pop block off of free list

SYNOPSIS
     #include <blkio.h>

     int bflpop(bp, bnp)
     BLKFILE *bp;
     bpos_t *bnp;

DESCRIPTION
     The bflpop function gets a block out of the free list of the
     block file associated with BLKFILE pointer bp.  If the free list
     is empty, the block number of the first block past the end of
     file is retrieved.  The block number of the new block is returned
     in the location pointed to by bnp.

     Note that if the free list is empty, the same end block will be
     retrieved by flpop until that block is written, extending the
     file length.

     bflpop will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       bnp is the NULL pointer.
     [BEEOF]        bp is empty.
     [BEEOF]        The free list head past points past the end of the
                    file.
     [BENFL]        bp does not have a free list.
     [BENOPEN]      bp is not open for writing.

SEE ALSO
     bflpush.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

NOTES
     To use the free list functions in the blkio library, the first
     element of the file header must be the free list head with type
     bpos_t.  This must be initialized to 0 immediately after the file
     is created and not accessed afterward except using bflpop and
     bflpush.  Also, the block size must be at least sizeof(bpos_t).

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bflpop(BLKFILE *bp, bpos_t *bnp)
#else
int bflpop(bp, bnp)
BLKFILE *bp;
bpos_t *bnp;
#endif
{
	bpos_t oldflhno = 0;
	bpos_t newflhno = 0;

	/* validate arguments */
	if (!b_valid(bp) || bnp == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open for writing */
	if (!(bp->flags & BIOWRITE)) {
		errno = BENOPEN;
		return -1;
	}

	/* check if no free list */
	if (bp->hdrsize < sizeof(oldflhno) || bp->blksize < sizeof(oldflhno)) {
		errno = BENFL;
		return -1;
	}

	/* check if header not yet written */
	if (bp->endblk < 1) {
		errno = BEEOF;
		return -1;
	}

	/* get block number of current free list head */
	if (bgethf(bp, (size_t)0, &oldflhno, sizeof(oldflhno)) == -1) {
		BEPRINT;
		return -1;
	}
	if (oldflhno >= bp->endblk) {
		errno = BEEOF;
		return -1;
	}

	/* if free list empty, get new block at end of file */
	if (oldflhno == 0) {
		oldflhno = bp->endblk;
	} else {	/* else get new free list head */
		if (bgetbf(bp, oldflhno, (size_t)0, &newflhno, sizeof(newflhno)) == -1) {
			BEPRINT;
			return -1;
		}
		if (bputhf(bp, (size_t)0, &newflhno, sizeof(newflhno)) == -1) {
			BEPRINT;
			return -1;
		}
	}

	/* load return argument */
	*bnp = oldflhno;

	return 0;
}
