/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)bcloseal.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#ifdef AC_STDDEF
#include <stddef.h>
#endif

/* local headers */
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bcloseall - close all block files

SYNOPSIS
     #include <blkio.h>

     void bcloseall()

DESCRIPTION
     The bcloseall function closes all open block files, flushing the
     buffers.  Any program using the blkio library should register
     bcloseall, with the ANSI C function atexit, to be called
     automatically on termination.  This will prevent the loss of
     buffered data that has not yet been written to the file.

     If the atexit function is not available, bexit should be used
     everywhere in place of exit.

SEE ALSO
     bclose, bexit.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
void bcloseall(void)
#else
void bcloseall()
#endif
{
	BLKFILE *bp = NULL;

	/* close all open block files */
	for (bp = biob; bp < (biob + BOPEN_MAX); ++bp) {
		if (bp->flags & BIOOPEN) {
			if (bclose(bp) == -1) {
				BEPRINT;
			}
		}
	}

	return;
}
