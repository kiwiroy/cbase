/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbreccnt.c	1.5 - 91/09/23" */

/*man---------------------------------------------------------------------------
NAME
     cbreccnt - cbase record count

SYNOPSIS
     #include <cbase.h>

     unsigned long cbreccnt(cbp)
     cbase_t *cbp;

DESCRIPTION
     cbreccnt returns the number of records in cbase cbp.  If cbp does
     not point to a valid open cbase, the results are undefined.
     cbreccnt is a macro.

------------------------------------------------------------------------------*/
/* cbreccnt is defined in cbase.h. */
