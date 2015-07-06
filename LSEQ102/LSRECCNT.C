/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lsreccnt.c	1.5 - 91/09/23" */

/*man---------------------------------------------------------------------------
NAME
     lsreccnt - lseq record count

SYNOPSIS
     #include <lseq.h>

     unsigned long lsreccnt(lsp)
     lseq_t *lsp;

DESCRIPTION
     lsreccnt returns the number of records in lseq lsp.  If lsp does
     not point to a valid open lseq, the results are undefined.
     lsreccnt is a macro.

------------------------------------------------------------------------------*/
/* lsreccnt defined in lseq.h. */
