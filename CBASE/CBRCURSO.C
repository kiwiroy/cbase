/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbrcurso.c	1.5 - 91/09/23" */

/*man---------------------------------------------------------------------------
NAME
     cbrcursor - cbase record cursor

SYNOPSIS
     #include <cbase.h>

     void *cbrcursor(cbp)
     cbase_t *cbp;

DESCRIPTION
     cbrcursor is used to determine if the cursor for cbase cbp is
     currently positioned on a record or on null.  If the cursor is
     on null, NULL is returned.  If the cursor is positioned to a
     record, a non-NULL value is returned.  If cbp does not point to a
     valid open cbase, the results are undefined.  cbrcursor is a
     macro.

SEE ALSO
     cbkcursor, cbrecfirst, cbreclast, cbrecnext, cbrecprev.

------------------------------------------------------------------------------*/
/* cbrcursor is defined in cbase.h. */
