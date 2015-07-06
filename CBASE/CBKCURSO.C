/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbkcurso.c	1.5 - 91/09/23" */

/*man---------------------------------------------------------------------------
NAME
     cbkcursor - cbase key cursor

SYNOPSIS
     #include <cbase.h>

     void *cbkcursor(cbp, field)
     cbase_t *cbp;
     int field;

DESCRIPTION
     cbkcursor is used to determine if the key cursor of the specified
     field in cbase cbp is currently positioned on a key or on null.
     If the cursor is on null, NULL is returned.  If the cursor is
     positioned to a key, a non-NULL value is returned.  If cbp does
     not point to a valid open cbase, or if field is not a key field,
     the results are undefined.  cbkcursor is a macro.

SEE ALSO
     cbkeyfirst, cbkeylast, cbkeynext, cbkeyprev, cbrcursor.

------------------------------------------------------------------------------*/
/* cbkcursor is defined in cbase.h. */
