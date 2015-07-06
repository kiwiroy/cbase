/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbexport.c	1.5 - 91/09/23" */

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

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cbexport - export cbase data

SYNOPSIS
     #include <cbase.h>

     int cbexport(cbp, filename)
     cbase_t *cbp;
     const char *filename;

DESCRIPTION
     The cbexport function exports all records in cbase cbp to a
     printable file.  filename points to a character string that
     contains the name of the file to receive the data.  If the file
     does not exist, it is created.  If the file exists, it is
     truncated to zero length before the data is exported.

     The exported file format is as follows:

          o Each record is terminated by a newline ('\\n').
          o The fields in a record are delimited by vertical
            bars ('|').
          o Each field contains only printable characters.
          o If a field contains the field delimiter
            character, that character is replaced with \\F.
          o The individual elements of array data types are
            exported as individual fields.

     cbexport will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [EINVAL]       filename is the NULL pointer.
     [CBELOCK]      cbp is not read-locked.
     [CBENOPEN]     cbp is not open.
     [CBEPRFILE]    Error writing to printable file.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbexport(cbase_t *cbp, const char *filename)
#else
int cbexport(cbp, filename)
cbase_t *cbp;
const char *filename;
#endif
{
	int	rs	= 0;
	int	terrno	= 0;
	FILE *	fp	= NULL;
	void *	buf	= NULL;
	int	field	= 0;
	size_t	fldos	= 0;
	size_t	fldlen	= 0;
	int	fldtype	= 0;

	/* validate arguments */
	if (!cb_valid(cbp) || filename == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* check if not read locked */
	if (!(cbp->flags & CB_RDLCK)) {
		errno = CBELOCK;
		return -1;
	}

	/* truncate or create file for writing */
	fp = fopen(filename, "w");
	if (fp == NULL) {
		return -1;
	}

	/* check if cbp is empty */
	if (cbreccnt(cbp) == 0) {
		if (fclose(fp) == EOF) {
			CBEPRINT;
			return -1;
		}
		return 0;
	}

	/* position record cursor to first record */
	if (cbrecfirst(cbp) == -1) {
		CBEPRINT;
		terrno = errno;
		fclose(fp);
		errno = terrno;
		return -1;
	}

	/* export all records to printable file */
	buf = calloc((size_t)1, cbrecsize(cbp));
	if (buf == NULL) {
		CBEPRINT;
		fclose(fp);
		errno = ENOMEM;
		return -1;
	}
	while (cbrcursor(cbp) != NULL) {
		if (cbgetr(cbp, buf) == -1) {	/* read current record */
			CBEPRINT;
			terrno = errno;
			free(buf);
			fclose(fp);
			errno = terrno;
			return -1;
		}
		/* output each field of current record */
		for (field = 0; field < cbp->fldc; ++field) {
			fldos = cbp->fldv[field].offset;
			fldlen = cbp->fldv[field].len;
			fldtype = cbp->fldv[field].type;
			/* write field to printable file */
			if ((*cbexpv[fldtype])(fp, (char *)buf + fldos, fldlen) == -1) {
				CBEPRINT;
				free(buf);
				fclose(fp);
				errno = CBEPRFILE;
				return -1;
			}
			if (field < cbp->fldc - 1) {
				rs = fputc(EXPFLDDLM, fp);
			} else {
				rs = fputc(EXPRECDLM, fp);
			}
			if (rs == EOF) {
				CBEPRINT;
				terrno = errno;
				free(buf);
				fclose(fp);
				errno = terrno;
				return -1;
			}
		}
		if (cbrecnext(cbp) == -1) {
			CBEPRINT;
			terrno = errno;
			free(buf);
			fclose(fp);
			errno = terrno;
			return -1;
		}
	}
	free(buf);
	buf = NULL;

	/* close file */
	if (fclose(fp) == EOF) {
		CBEPRINT;
		return -1;
	}

	return 0;
}
