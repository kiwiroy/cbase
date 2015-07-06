/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)bflpush.c	1.5 - 91/09/23" */

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

/* local headers */
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bflpush - push block onto free list

SYNOPSIS
     #include <blkio.h>

     int bflpush(bp, bnp)
     BLKFILE *bp;
     const bpos_t *bnp;

DESCRIPTION
     The bflpush function puts the block number pointed to by bnp into
     the free list of the block file associated with BLKFILE pointer
     bp.  If the value pointed to by bnp is one block past the end of
     the file, it is accepted but not actually added to the list.

     bflpush will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       bnp is the NULL pointer.
     [EINVAL]       bnp points to a value less than 1.
     [BEEOF]        bp is empty.
     [BEEOF]        bnp points to a value farther than one block past
                    the end of file.
     [BENFL]        bp does not have a free list.
     [BENOPEN]      bp is not open for writing.

SEE ALSO
     bflpop.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

NOTES
     To use the free list functions in the blkio library, the first
     element of the file header must be the free list head with type
     bpos_t.  This must be initialized to 0 immediately after the file
     is created and not accessed afterward except using bflpush and
     bflpush.  Also, the block size must be at least sizeof(bpos_t).

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bflpush(BLKFILE *bp, const bpos_t *bnp)
#else
int bflpush(bp, bnp)
BLKFILE *bp;
const bpos_t *bnp;
#endif
{
	bpos_t	oldflhno = 0;	/* old free list head number */
	bpos_t	newflhno = 0;	/* new free list head number */
	void *	buf	= NULL;

	/* validate arguments */
	if (!b_valid(bp) || bnp == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (*bnp < 1) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
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

	/* check if block past endblk */
	if (*bnp > bp->endblk) {
		errno = BEEOF;
		return -1;
	}

	/* check if block is endblk */
	if (*bnp == bp->endblk) {
		return 0;
	}

	/* get block number of current free list head */
	if (bgethf(bp, (size_t)0, &oldflhno, sizeof(oldflhno)) == -1) {
		BEPRINT;
		return -1;
	}

	/* link to rest of free list */
	newflhno = *bnp;
	buf = calloc((size_t)1, bp->blksize);
	if (buf == NULL) {
		BEPRINT;
		errno = ENOMEM;
		return -1;
	}
	memcpy(buf, &oldflhno, sizeof(oldflhno));
	if (bputb(bp, newflhno, buf) == -1) {
		BEPRINT;
		free(buf);
		buf = NULL;
		return -1;
	}
	free(buf);
	buf = NULL;

	/* write new free list head number to header */
	if (bputhf(bp, (size_t)0, &newflhno, sizeof(newflhno)) == -1) {
		BEPRINT;
		return -1;
	}

	return 0;
}
