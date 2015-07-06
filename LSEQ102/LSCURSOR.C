/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lscursor.c	1.5 - 91/09/23" */

/*man---------------------------------------------------------------------------
NAME
     lscursor - lseq cursor

SYNOPSIS
     #include <lseq.h>

     void *lscursor(lsp)
     lseq_t *lsp;

DESCRIPTION
     lscursor is used to determine if the cursor for lseq lsp is
     currently positioned on a record or is null.  If the cursor is
     set to null, the NULL pointer is returned.  If the cursor is
     positioned to a record, a non-NULL value is returned.  If lsp
     does not point to a valid open lseq , the results are undefined.
     lscursor is a macro.

SEE ALSO
     lsfirst, lslast, lsnext, lsprev.

------------------------------------------------------------------------------*/
/* lscursor is defined in lseq.h. */
