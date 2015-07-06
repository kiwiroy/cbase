/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)bops.c	1.5 - 91/09/23" */

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

/* non-ansi headers */
#include <bool.h>

/* local headers */
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     b_alloc - allocate memory for block file

SYNOPSIS
     #include "blkio_.h"

     int b_alloc(bp)
     BLKFILE *bp;

DESCRIPTION
     The b_alloc function allocates the memory needed by bp.  The
     memory is initialized to all zeros.  A call to b_alloc should
     normally be followed by a call to b_initlist to construct the
     linked list for LRU replacement.

     b_alloc will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [ENOMEM]       Enough memory is not available for
                    allocation by the calling process.
     [BENOPEN]      bp is not open.

SEE ALSO
     b_free, b_initlist.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int b_alloc(BLKFILE *bp)
#else
int b_alloc(bp)
BLKFILE *bp;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp)) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOOPEN)) {
		BEPRINT;
		errno = BENOPEN;
		return -1;
	}

	/* check for memory leak */
	if (bp->blockp != NULL || bp->blkbuf != NULL && !(bp->flags & BIOUSRBUF)) {
		BEPRINT;
		errno = BEPANIC;
		return -1;
	}
#endif
	/* check if not buffered */
	if (bp->bufcnt == 0) {
		return 0;
	}

	/* allocate buffer storage (one extra block structure for header) */
	bp->blockp = (block_t *)calloc(bp->bufcnt + 1, sizeof(block_t));
	if (bp->blockp == NULL) {
		BEPRINT;
		errno = ENOMEM;
		return -1;
	}
	if (!(bp->flags & BIOUSRBUF)) {
		bp->blkbuf = calloc((size_t)1, bp->hdrsize + bp->bufcnt * bp->blksize);
		if (bp->blkbuf == NULL) {
			BEPRINT;
			free(bp->blockp);
			bp->blockp = NULL;
			errno = ENOMEM;
			return -1;
		}
	}

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_blockp - pointer to block structure

SYNOPSIS
     #include "blkio_.h"

     block_t *b_blockp(bp, i)
     BLKFILE *bp;
     size_t i;

DESCRIPTION
     b_blockp returns a pointer to the ith block structure in the
     buffer list of block file bp.  If bp is not a valid open block
     file or i is not in the range [0..bp->bufcnt] the results are
     undefined.  b_blockp is a macro.

------------------------------------------------------------------------------*/
/* b_blockp is defined in blkio_.h */

/*man---------------------------------------------------------------------------
NAME
     b_blkbuf - pointer to block buffer

SYNOPSIS
     #include "blkio_.h"

     void *b_blkbuf(bp, i)
     BLKFILE *bp;
     size_t i;

DESCRIPTION
     b_blkbuf returns a pointer to the ith buffer in the buffer list
     of block file bp.  A value of zero for i indicates the header
     buffer.  If bp is not a valid BLKFILE pointer to an open file or
     i is not in the range [0..bp->bufcnt] the results are undefined.
     b_blkbuf is a macro.

------------------------------------------------------------------------------*/
/* b_blkbuf is defined in blkio_.h */

/*man---------------------------------------------------------------------------
NAME
     b_free - free memory allocated for block file

SYNOPSIS
     #include "blkio_.h"

     void b_free(bp)
     BLKFILE *bp;

DESCRIPTION
     The b_free function frees all memory allocated for block file bp.
     If bp has a user-supplied buffer storage area, it is disconnected
     from bp but is not freed.  On return from b_free, bp->blockp and
     bp->blkbuf will be NULL.

SEE ALSO
     b_alloc.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
void b_free(BLKFILE *bp)
#else
void b_free(bp)
BLKFILE *bp;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp)) {
		BEPRINT;
		return;
	}
#endif
	/* free memory */
	if (!(bp->flags & BIOUSRBUF)) {		/* if not user-supplied buf */
		if (bp->blkbuf != NULL) {		/* free buffer */
			free(bp->blkbuf);
		}
	}
	bp->blkbuf = NULL;			/* disconnect buffer */
	if (bp->blockp != NULL) {
		free(bp->blockp);
		bp->blockp = NULL;
	}

	return;
}

/*man---------------------------------------------------------------------------
NAME
     b_get - get block from block file

SYNOPSIS
     #include "blkio_.h"

     int b_get(bp, i)
     BLKFILE *bp;
     size_t i;

DESCRIPTION
     The b_get function reads a block from block file bp into the ith
     buffer for that block file.  b_get reads the block identified by
     the block number field of the block structure associated with
     buffer i (i.e., b_blockp(bp, i)->bn); the zeroth buffer is
     always used for the header.  The read flag is set and all others
     cleared for buffer i.

     b_get will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       i is not in the range [0..bp->bufcnt].
     [BEEOF]        End of file occured before end of block.
     [BENBUF]       bp is not buffered.

SEE ALSO
     b_put.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int b_get(BLKFILE *bp, size_t i)
#else
int b_get(bp, i)
BLKFILE *bp;
size_t i;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp)) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOOPEN)) {
		BEPRINT;
		errno = BENOPEN;
		return -1;
	}

	/* check if file is not buffered */
	if (bp->bufcnt == 0) {
		BEPRINT;
		errno = BENBUF;
		return -1;
	}

	/* validate arguments */
	if (i > bp->bufcnt) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if block number set */
	if ((b_blockp(bp, i)->bn == 0) && (i != 0)) {
		BEPRINT;
		errno = BEPANIC;
		return -1;
	}

	/* check if block needs to be written to disk */
	if (b_blockp(bp, i)->flags & BLKWRITE) {
		BEPRINT;
		errno = BEPANIC;
		return -1;
	}
#endif
	/* read block from file */
	if (i == 0) {
		if (b_ugetf(bp, (bpos_t)0, (size_t)0, b_blkbuf(bp, (size_t)0), bp->hdrsize) == -1) {
			BEPRINT;
			return -1;
		}
	} else {
		if (b_ugetf(bp, b_blockp(bp, i)->bn, (size_t)0, b_blkbuf(bp, i), bp->blksize) == -1) {
			BEPRINT;
			return -1;
		}
	}

	/* set read flag and clear all others */
	b_blockp(bp, i)->flags = BLKREAD;

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_initlist - build linked list

SYNOPSIS
     #include "blkio_.h"

     int b_initlist(bp)
     BLKFILE *bp;

DESCRIPTION
     The b_initlist function builds the linked list of buffers for
     block file bp.  The buffer contents are deleted in the process.
     A call to b_initlist should normally follow a call to b_alloc.

     b_initlist will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid block file.

SEE ALSO
     b_alloc.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int b_initlist(BLKFILE *bp)
#else
int b_initlist(bp)
BLKFILE *bp;
#endif
{
	size_t i = 0;

#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp)) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOOPEN)) {
		BEPRINT;
		errno = BENOPEN;
		return -1;
	}
#endif
	/* initialize head and tail of list */
	bp->most = 0;
	bp->least = 0;

	/* check if not buffered */
	if (bp->bufcnt == 0) {
		return 0;
	}

	/* initialize linked list */
	bp->most = bp->bufcnt;
	bp->least = 1;
	for (i = 1; i <= bp->bufcnt; ++i) {
		b_blockp(bp, i)->bn = 0;
		b_blockp(bp, i)->flags = 0;
		b_blockp(bp, i)->more = i + 1;
		b_blockp(bp, i)->less = i - 1;
	}
	b_blockp(bp, bp->most)->more = 0;
	b_blockp(bp, bp->least)->less = 0;

	/* initialize block structure for header */
	b_blockp(bp, (size_t)0)->bn = 0;
	b_blockp(bp, (size_t)0)->flags = 0;
	b_blockp(bp, (size_t)0)->more = 0;
	b_blockp(bp, (size_t)0)->less = 0;

	/* scrub buffer storage area */
	memset(bp->blkbuf, 0, bp->hdrsize + bp->bufcnt * bp->blksize);

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_mkmru - make most recently used block

SYNOPSIS
     #include "blkio_.h"

     int b_mkmru(bp, i)
     BLKFILE *bp;
     size_t i;

DESCRIPTION
     The b_mkmru function moves the ith block in the buffer list to
     the most recently used end of the buffer list.

     b_mkmru will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid block file.
     [EINVAL]       i is not in the range [1..bp->bufcnt].
     [BENBUF]       bp is not buffered.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int b_mkmru(BLKFILE *bp, size_t i)
#else
int b_mkmru(bp, i)
BLKFILE *bp;
size_t i;
#endif
{
	size_t more = 0;
	size_t less = 0;

#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp)) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOOPEN)) {
		BEPRINT;
		errno = BENOPEN;
		return -1;
	}

	/* check if file is not buffered */
	if (bp->bufcnt == 0) {
		BEPRINT;
		errno = BENBUF;
		return -1;
	}

	/* validate arguments */
	if (i < 1 || i > bp->bufcnt) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}
#endif
	/* check block addresses */
	more = b_blockp(bp, i)->more;
	less = b_blockp(bp, i)->less;
	if (more > bp->bufcnt || less > bp->bufcnt) {
		BEPRINT;
		errno = BEPANIC;
		return -1;
	}

	/* check if already most recently used */
	if (more == 0) {
		return 0;
	}

	/* remove block i from linked list */
	b_blockp(bp, more)->less = less;
	if (less != 0) {
		b_blockp(bp, less)->more = more;
	} else {
		bp->least = more;
	}

	/* connect ith block as most recently used */
	b_blockp(bp, i)->more = 0;
	b_blockp(bp, i)->less = bp->most;
	b_blockp(bp, bp->most)->more = i;
	bp->most = i;

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_put - put block into block file

SYNOPSIS
     #include "blkio_.h"

     int b_put(bp, i)
     BLKFILE *bp;
     size_t i;

DESCRIPTION
     The b_put function writes a block from the ith buffer of block
     file bp to the file.  b_put writes to the block identified by
     the block number field of the block structure associated with
     buffer i (i.e., b_blockp(bp, i)->bn); the zeroth buffer is
     always used for the header.  If the write flag is not set,
     nothing is written.  After writing, the write flag is cleared.

     b_put will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid block file.
     [EINVAL]       i is not in the range [0..bp->bufcnt].
     [BENBUF]       bp is not buffered.

SEE ALSO
     b_get.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int b_put(BLKFILE *bp, size_t i)
#else
int b_put(bp, i)
BLKFILE *bp;
size_t i;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp)) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOOPEN)) {
		BEPRINT;
		errno = BENOPEN;
		return -1;
	}

	/* check if file is not buffered */
	if (bp->bufcnt == 0) {
		BEPRINT;
		errno = BENBUF;
		return -1;
	}

	/* validate arguments */
	if (i > bp->bufcnt) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}
#endif
	/* check if block doesn't need to be written to disk */
	if (!(b_blockp(bp, i)->flags & BLKWRITE)) {
		return 0;
	}

	/* check for inconsistent read flag setting */
	if (!(b_blockp(bp, i)->flags & BLKREAD)) {
		BEPRINT;
		errno = BEPANIC;
		return 0;
	}

	/* write block to disk */
	if (i == 0) {
		if (b_uputf(bp, (bpos_t)0, (size_t)0, b_blkbuf(bp, (size_t)0), bp->hdrsize) == -1) {
			BEPRINT;
			return -1;
		}
	} else {
		if (b_uputf(bp, b_blockp(bp, i)->bn, (size_t)0, b_blkbuf(bp, i), bp->blksize) == -1) {
			BEPRINT;
			return -1;
		}
	}

	/* clear all but read flag */
	b_blockp(bp, i)->flags = BLKREAD;

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_valid - validate block file pointer

SYNOPSIS
     #include "blkio_.h"

     bool b_valid(bp)
     BLKFILE *bp;

DESCRIPTION
     The b_valid function determines if bp is a valid BLKFILE pointer.
     If valid, the TRUE is returned.  If not, then FALSE is returned.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
bool b_valid(const BLKFILE *bp)
#else
bool b_valid(bp)
const BLKFILE *bp;
#endif
{
	if (bp < biob || bp > (biob + BOPEN_MAX - 1)) {
		return FALSE;
	}
	if (((char *)bp - (char *)biob) % sizeof(*biob) != 0) {
		return FALSE;
	}

	return TRUE;
}
