/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btcursor.c	1.5 - 91/09/23" */

/*man---------------------------------------------------------------------------
NAME
     btcursor - btree cursor

SYNOPSIS
     #include <btree.h>

     void *btcursor(btp)
     btree_t *btp;

DESCRIPTION
     btcursor is used to determine if the cursor for btree btp is
     currently positioned on a key or is null.  If the cursor is set
     to null, the NULL pointer is returned.  If the cursor is
     positioned to a key, a non-NULL value is returned.  If btp does
     not point to a valid open btree, the results are undefined.
     btcursor is a macro.

SEE ALSO
     btfirst, btlast, btnext, btgetk, btprev.

------------------------------------------------------------------------------*/
/* btcursor defined in btree.h. */
