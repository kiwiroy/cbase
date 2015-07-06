/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btkeycnt.c	1.5 - 91/09/23" */

/*man---------------------------------------------------------------------------
NAME
     btkeycnt - btree key count

SYNOPSIS
     #include <btree.h>

     unsigned long btkeycnt(btp)
     btree_t *btp;

DESCRIPTION
     btkeycnt returns the number of keys in btree btp.  If btp does
     not point to a valid open btree, the results are undefined.
     btkeycnt is a macro.

------------------------------------------------------------------------------*/
/* btkeycnt is defined in btree.h. */
