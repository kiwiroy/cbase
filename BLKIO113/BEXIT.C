/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)bexit.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#ifdef AC_STDLIB
#include <stdlib.h>
#endif

/* local headers */
#include "blkio_.h"

/*man---------------------------------------------------------------------------
NAME
     bexit - block file exit

SYNOPSIS
     #include <blkio.h>

     void bexit(status)
     int status;

DESCRIPTION
     The bexit function is for use with the blkio library in place of
     exit.  It closes all open block files,  flushing the buffers,
     then calls exit.

SEE ALSO
     bclose.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
void bexit(int status)
#else
void bexit(status)
int status;
#endif
{
	bcloseall();

	exit(status);
}
