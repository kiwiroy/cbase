/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lscreate.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <blkio.h>

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     lscreate - create an lseq

SYNOPSIS
     int lscreate(filename, recsize)
     const char *filename;
     size_t recsize;

DESCRIPTION
     The lscreate function creates the file named by filename as an
     lseq.

     recsize is the size of the records in the lseq.

     lscreate will fail if one or more of the following is true:

     [EINVAL]       filename is the NULL pointer.
     [EINVAL]       recsize is less than 1.
     [LSEMFILE]     Too many open lseqs.  The maximum
                    is defined as LSOPEN_MAX in lseq.h.

SEE ALSO
     lsopen.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lscreate(const char * filename, size_t recsize)
#else
int lscreate(filename, recsize)
const char * filename;
size_t recsize;
#endif
{
	int	terrno	= 0;
	lseq_t *lsp	= NULL;

	/* validate input parameters */
	if (filename == NULL || recsize < sizeof(lspos_t)) {
		errno = EINVAL;
		return -1;
	}

	/* find free slot in lsb table */
	for (lsp = lsb; lsp < (lsb + LSOPEN_MAX); lsp++) {
		if (!(lsp->flags & LSOPEN)) {
			break;		/* found */
		}
	}
	if (lsp >= lsb + LSOPEN_MAX) {
		errno = LSEMFILE;
		return -1;		/* no free slots */
	}

	/* load lseq_t structure */
	lsp->lshdr.flh = NIL;
	lsp->lshdr.recsize = recsize;
	lsp->lshdr.flags = 0;
	lsp->lshdr.first = NIL;
	lsp->lshdr.last = NIL;
	lsp->lshdr.reccnt = 0;
	lsp->bp = NULL;
	lsp->flags = LSREAD | LSWRITE;
	lsp->clspos = NIL;
	lsp->clsrp = NULL;
	if (ls_alloc(lsp) == -1) {
		LSEPRINT;
		memset(lsp, 0, sizeof(*lsb));
		lsp->flags = 0;
		return -1;
	}

	/* create file */
	lsp->bp = bopen(filename, "c", sizeof(lshdr_t), (size_t)1, (size_t)0);
	if (lsp->bp == NULL) {
		if (errno != EEXIST) LSEPRINT;
		ls_free(lsp);
		memset(lsp, 0, sizeof(*lsb));
		lsp->flags = 0;
		return -1;
	}

	/* write header to file */
	if (bputh(lsp->bp, &lsp->lshdr) == -1) {
		LSEPRINT;
		terrno = errno;
		bclose(lsp->bp);
		ls_free(lsp);
		memset(lsp, 0, sizeof(*lsb));
		lsp->flags = 0;
		errno = terrno;
		return -1;
	}

	/* close lsp */
	if (lsclose(lsp) == -1) {
		LSEPRINT;
		ls_free(lsp);
		return -1;
	}

	return 0;
}
