/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)bputh.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#ifdef AC_STDDEF
#include <stddef.h>
#endif

/* local headers */
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bputh - put the header into a block file

SYNOPSIS
     #include <blkio.h>

     int bputh(bp, buf)
     BLKFILE *bp;
     const void *buf;

DESCRIPTION
     The bputh function writes the the contents of buf into the header
     of block file bp.  buf must point to a storage area at least as
     large as the header size for bp.

     bputh will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid block file.
     [EINVAL]       buf is the NULL pointer.
     [BENOPEN]      bp is not open for writing.

SEE ALSO
     bgeth, bputhf, bputb.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bputh(BLKFILE *bp, const void *buf)
#else
int bputh(bp, buf)
BLKFILE *bp;
const void *buf;
#endif
{
	return bputhf(bp, (size_t)0, buf, bp->hdrsize);
}
