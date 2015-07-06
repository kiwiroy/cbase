/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbimport.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#include <stdio.h>
#ifdef AC_STDLIB
#include <stdlib.h>
#endif
#ifdef AC_STRING
#include <string.h>
#endif

/* non-ansi headers */
#include <bool.h>

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cbimport - cbase import

SYNOPSIS
     #include <cbase.h>

     int cbimport(cbp, filename)
     cbase_t *cbp;
     const char *filename;

DESCRIPTION
     The cbimport function imports all records from a text file to
     cbase cbp.  filename points to a character string that contains
     the name of the text file.  See cbexport for a definition of the
     file format.

     If a record containing an illegal duplicate key is encountered
     during the import, that record is skipped and the import
     continues with the subsequent record.  On successful completion
     of the remainder of the import, a value of -1 is returned with
     errno set to CBEDUP.  Whether or not the calling program should
     treat this as an error condition is application dependent.

     cbimport will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [EINVAL]       filename is the NULL pointer.
     [ENOENT]       filename does not exist.
     [CBEDUP]       An illegal duplicate key was encountered.
     [CBELOCK]      cbp is not write-locked.
     [CBEPRFILE]    Error reading from text file.
     [CBEPRFILE]    Invalid text file format.

SEE ALSO
     cbexport.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbimport(cbase_t *cbp, const char *filename)
#else
int cbimport(cbp, filename)
cbase_t *cbp;
const char *filename;
#endif
{
	FILE *	fp	= NULL;		/* import stream */
	int	c	= 0;		/* int character */
	void *	buf	= NULL;		/* input buffer */
	bool	dupflag	= FALSE;	/* illegal duplicate key flag */
	bool	errflag	= FALSE;	/* error flag */
	int	field	= 0;		/* field number */
	size_t	fldos	= 0;		/* field offset */
	size_t	fldlen	= 0;		/* field length */
	int	fldtype	= 0;		/* field data type */
	int	terrno	= 0;		/* tmp errno */

	/* validate arguments */
	if (!cb_valid(cbp) || filename == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* open file for reading */
	fp = fopen(filename, "r");
	if (fp == NULL) {
		return -1;
	}

	/* import all records from text file */
	buf = calloc((size_t)1, cbrecsize(cbp));
	if (buf == NULL) {
		CBEPRINT;
		fclose(fp);
		errno = ENOMEM;
		return -1;
	}
	for (;;) {
		/* check for eof */
		if ((c = getc(fp)) == EOF) {
			if (ferror(fp)) {
				CBEPRINT;
				terrno = errno;
				errflag = TRUE;
				break;
			}
			if (feof(fp)) break;
		}
		if (ungetc(c, fp) == EOF) {
			CBEPRINT;
			terrno = errno;
			errflag = TRUE;
			break;
		}
		memset(buf, 0, cbrecsize(cbp));
		/* input each field of new record */
		for (field = 0; field < cbp->fldc; ++field) {
			fldos = cbp->fldv[field].offset;
			fldlen = cbp->fldv[field].len;
			fldtype = cbp->fldv[field].type;
			/* read field from text file */
			if ((*cbimpv[fldtype])(fp, (char *)buf + fldos, fldlen) == -1) {
				CBEPRINT;
				terrno = CBEPRFILE;
				errflag = TRUE;
				break;
			}
		}
		/* add record to database */
		if (cbinsert(cbp, buf) == -1) {
			if (errno == CBEDUP) {
				dupflag = TRUE;
			} else {
				terrno = errno;
				errflag = TRUE;
				break;
			}
		}
	}
	free(buf);
	buf = NULL;
	if (errflag) {
		fclose(fp);
		errno = terrno;
		return -1;
	}

	/* close file */
	if (fclose(fp) == EOF) {
		CBEPRINT;
		return -1;
	}

	/* notify if illegal duplicate key */
	if (dupflag) {
		errno = CBEDUP;
		return -1;
	}

	return 0;
}
