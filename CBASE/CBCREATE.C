/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbcreate.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#include <stdio.h>

/* library headers */
#include <blkio.h>
#include <btree.h>
#include <lseq.h>

/* local headers */
#include "cbase_.h"

/* btree field definition list */
static btfield_t btfldv[] = {
	{
		0,
		0,
		NULL,
		BT_FASC
	},
	{
		0,
		sizeof(cbrpos_t),
		cbrposcmp,
		BT_FASC
	},
};

/*man---------------------------------------------------------------------------
NAME
     cbcreate - create a cbase

SYNOPSIS
     #include <cbase.h>

     int cbcreate(cbname, recsize, fldc, fldv)
     const char *cbname;
     size_t recsize;
     int fldc;
     const cbfield_t fldv[];

DESCRIPTION
     The cbcreate function creates a cbase.  cbname points to a
     character string that contains the name of the cbase to be
     created.  cbname is used as the name of the data file containing
     the records in the cbase.

     recsize specifies the size of the records in the cbase.

     fldc is the field count.  It specifies the number of fields in
     the records stored in this cbase.  fldv is an array of field
     definition structures.  fldv must have fldc elements.  The field
     definition structure is defined in <cbase.h> as type cbfield_t.
     It has the following members.

          size_t offset;      /* offset of field in record *\/
          size_t len;         /* field length *\/
          int type;           /* field data type *\/
          int flags;          /* flags *\/
          char *filename;     /* name of key file *\/

     offset and len specify the location and length of the field,
     respectively.  type is the data type for the field; see cbase
     manual entry for a list of the predefined data types, and
     the cbase Programmer's Guide for information on adding new data
     types.  filename is the name of the file to be used for key
     storage.  flags values are constructed by bitwise OR-ing flags
     from the following list:

     CB_FKEY        Field is a key.
     CB_FUNIQ       Only for use with CB_FKEY.  Indicates
                    that the keys must be unique.

     The fields in the field definition list must be in order,
     starting with the first field in the record.

     cbcreate will fail if one or more of the following is true:

     [EEXIST]       Either the record file or one of
                    the key files exists.
     [EINVAL]       cbname is the NULL pointer.
     [EINVAL]       recsize is less than sizeof(cbrpos_t).
     [EINVAL]       fldc is less than 1.
     [EINVAL]       fldv is the NULL pointer.
     [EINVAL]       fldv  contains an invalid field
                    definition.

SEE ALSO
     cbopen.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbcreate(const char *cbname, size_t recsize, int fldc, const cbfield_t fldv[])
#else
int cbcreate(cbname, recsize, fldc, fldv)
const char *cbname;
size_t recsize;
int fldc;
const cbfield_t fldv[];
#endif
{
	int	terrno	= 0;
	int	i	= 0;

	/* validate arguments */
	if (cbname == NULL || recsize < sizeof(cbrpos_t)) {
		errno = EINVAL;
		return -1;
	}
	if (!cb_fvalid(recsize, fldc, fldv)) {
		errno = EINVAL;
		return -1;
	}

	/* create data file */
	if (lscreate(cbname, recsize) == -1) {
		if (errno != EEXIST) CBEPRINT;
		return -1;
	}

	/* create key files */
	for (i = 0; i < fldc; ++i) {
		if (fldv[i].flags & CB_FKEY) {
			btfldv[1].offset = btfldv[0].len = fldv[i].len;
			btfldv[0].cmp = cbcmpv[fldv[i].type];
			if (btcreate(fldv[i].filename, CBM, fldv[i].len + sizeof(cbrpos_t), 2, btfldv) == -1) {
				if (errno != EEXIST) CBEPRINT;
				terrno = errno;
				for (i--; i >= 0; i--) {	/* remove files */
					if (fldv[i].flags & CB_FKEY) {
						remove(fldv[i].filename);
					}
				}
				remove(cbname);
				errno = terrno;
				return -1;
			}
		}
	}

	return 0;
}
