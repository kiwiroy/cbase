/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbrecsiz.c	1.5 - 91/09/23" */

/*man---------------------------------------------------------------------------
NAME
     cbrecsize - cbase record size

SYNOPSIS
     #include <cbase.h>

     size_t cbrecsize(cbp)
     cbase_t *cbp;

DESCRIPTION
     cbrecsize returns the size of the records in cbase cbp.  It is
     not necessary that cbp be locked at the time cbrecsize is called,
     but a valid result is returned only if cbp has been locked at
     least once.

     If cbp does not point to a valid open cbase, the results are
     undefined.  cbrecsize is a macro.

------------------------------------------------------------------------------*/
/* cbrecsize is defined in cbase.h. */
