/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lsopen.c	1.5 - 91/09/23" */

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

/* lseq control structure table definition */
lseq_t lsb[LSOPEN_MAX];

/*man---------------------------------------------------------------------------
NAME
     lsopen - open an lseq

SYNOPSIS
     lseq_t *lsopen(filename, type)
     const char *filename;
     const char *type;

DESCRIPTION
     The lsopen function opens the file named by filename as an lseq.
     A pointer to the lseq_t structure associated with the file is
     returned.

     type is a character string having one of the following values:

          "r"            open for reading
          "r+"           open for update (reading and writing)

     lsopen will fail if one or more of the following is true:

     [EINVAL]       filename is the NULL pointer.
     [EINVAL]       type is not "r" or "r+".
     [ENOENT]       The named lseq file does not exist.
     [LSEMFILE]     Too many open lseqs.  The maximum
                    is defined as LSOPEN_MAX in lseq.h.

SEE ALSO
     lsclose, lscreate.

DIAGNOSTICS
     lsopen returns a NULL pointer on failure, and errno is set to
     indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
lseq_t *lsopen(const char *filename, const char *type)
#else
lseq_t *lsopen(filename, type)
const char *filename;
const char *type;
#endif
{
	lseq_t *	lsp	= NULL;

	/* validate input parameters */
	if (filename == NULL || type == NULL) {
		errno = EINVAL;
		return NULL;
	}

	/* find free slot in lsb table */
	for (lsp = lsb; lsp < lsb + LSOPEN_MAX; ++lsp) {
		if (!(lsp->flags & LSOPEN)) {
			break;		/* found */
		}
	}
	if (lsp >= lsb + LSOPEN_MAX) {
		errno = LSEMFILE;
		return NULL;		/* no free slots */
	}

	/* open file */
	if (strcmp(type, LS_READ) == 0) {
		lsp->flags = LSREAD;
	} else if (strcmp(type, LS_RDWR) == 0) {
		lsp->flags = LSREAD | LSWRITE;
	} else {
		errno = EINVAL;
		return NULL;
	}
	lsp->bp = bopen(filename, type, sizeof(lshdr_t), (size_t)1, (size_t)0);
	if (lsp->bp == NULL) {
#ifdef DEBUG
		if (errno != EACCES && errno != ENOENT) LSEPRINT;
#endif
		memset(lsp, 0, sizeof(*lsp));
		lsp->flags = 0;
		return NULL;
	}

	/* load lseq_t structure */
	memset(&lsp->lshdr, 0, sizeof(lsp->lshdr));	/* header */
	lsp->clspos = NIL;			/* cursor */
	lsp->clsrp = NULL;			/* current record pointer */

	return lsp;
}
