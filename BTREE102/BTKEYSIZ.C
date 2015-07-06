/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btkeysiz.c	1.5 - 91/09/23" */

/*man---------------------------------------------------------------------------
NAME
     btkeysize - btree key size

SYNOPSIS
     #include <btree.h>

     size_t btkeysize(btp)
     btree_t *btp;

DESCRIPTION
     btkeysize returns the size of keys in btree btp.  If btp does not
     point to a valid open btree, the results are undefined.
     btkeysize is a macro.

------------------------------------------------------------------------------*/
/* btkeysize is defined in btree.h. */
