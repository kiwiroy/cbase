/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbmkndx.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#ifdef AC_STDLIB
#include <stdlib.h>
#endif
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <blkio.h>
#include <btree.h>

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
     cbmkndx - create new cbase index

SYNOPSIS
     #include <cbase.h>

     int cbmkndx(cbp, field, flags, filename)
     cbase_t *cbp;
     int field;
     int flags;
     char *filename;

DESCRIPTION
     The cbmkndx function builds a new index for an existing cbase.
     The field to be indexed for cbp is specified by field.  flags
     values are constructed by bitwise OR-ing flags from the following
     list:

     CB_FKEY        Field is a key.
     CB_FUNIQ       Only for use with CB_FKEY.  Indicates
                    that the keys must be unique.

     CB_FKEY is assumed, and it is not necessary to set it in flags.
     filename is the name of the file where the index is to reside.

     cbmkndx does not return until the build is completed.

     cbmkndx will fail if one or more of the following is true:

     [CBELOCK]      cbp is not read-locked.
     [EEXIST]       The named field is already a key.
     [EEXIST]       A file by the specified name already exists.
     [EINVAL]       cbp is not a valid cbase pointer.
     [EINVAL]       field is not a valid field number for cbp.
     [EINVAL]       flags contains an invalid field flag.
     [EINVAL]       filename is the NULL pointer.

SEE ALSO
     cbcreate, cbrmndx.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbmkndx(cbase_t *cbp, int field, int flags, char *filename)
#else
int cbmkndx(cbp, field, flags, filename)
cbase_t *cbp;
int field;
int flags;
char *filename;
#endif
{
	cbrpos_t	cbrpos	= NIL;		/* record position */
	void *		key	= NULL;		/* key buffer address */
	void *		rec	= NULL;		/* record buffer address */
	int		ltype	= BT_UNLCK;	/* lock type */

	/* validate arguments */
	if (!cb_valid(cbp)) {
		errno = EINVAL;
		return -1;
	}
	if (field < 0 || field >= cbp->fldc) {
		errno = EINVAL;
		return -1;
	}
	if (flags & !(CB_FFLAGS) || filename == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* check lock status */
	ltype = cbgetlck(cbp);
	switch (ltype) {
	case CB_RDLCK:
		ltype = BT_RDLCK;
		break;
	case CB_WRLCK:
		ltype = BT_WRLCK;
		break;
	default:
		errno = CBELOCK;
		return -1;
	}

	/* check if already index */
	if (cbp->fldv[field].flags & CB_FKEY) {
		errno = EEXIST;
		return -1;
	}

	/* set flags and filename */
	cbp->fldv[field].flags = CB_FKEY | flags;
	cbp->fldv[field].filename = filename;

	/* create new index */
	btfldv[1].offset = btfldv[0].len = cbp->fldv[field].len;
	btfldv[0].cmp = cbcmpv[cbp->fldv[field].type];
	if (btcreate(filename, CBM, btfldv[0].len + sizeof(cbrpos_t), 2, btfldv) == -1) {
#ifdef DEBUG
		if (errno != EEXIST) CBEPRINT;
#endif
		return -1;
	}
	cbp->btpv[field] = btopen(filename, "r+", 2, btfldv);
	if (cbp->btpv[field] == NULL) {
		CBEPRINT;
		return -1;
	}
	if (btlock(cbp->btpv[field], BT_WRLCK) == -1) {
		CBEPRINT;
		return -1;
	}

	/* load index */
	if (cbreccnt(cbp) == 0) {
		return 0;
	}
	if (cbrecfirst(cbp) == -1) {
		CBEPRINT;
		return -1;
	}
	/* allocate record and key */
	rec = calloc((size_t)1, cbrecsize(cbp));
	if (rec == NULL) {
		CBEPRINT;
		errno = ENOMEM;
		return -1;
	}
	key = calloc((size_t)1, cbp->fldv[field].len + sizeof(cbrpos_t));
	if (key == NULL) {
		CBEPRINT;
		errno = ENOMEM;
		return -1;
	}
	while (cbrcursor(cbp) != NULL) {
		/* get record and position */
		if (cbgetr(cbp, rec) == -1) {
			CBEPRINT;
			free(rec);
			free(key);
			return -1;
		}
		if (cbgetrcur(cbp, &cbrpos) == -1) {
			CBEPRINT;
			free(rec);
			free(key);
			return -1;
		}
		/* construct (key, record position) pair */
		memcpy(key, (char *)rec + cbp->fldv[field].offset, cbp->fldv[field].len);
		memcpy((char *)key + cbp->fldv[field].len, &cbrpos, sizeof(cbrpos_t));
		/* insert key */
		if (btinsert(cbp->btpv[field], key) == -1) {
			CBEPRINT;
			free(rec);
			free(key);
			return -1;
		}
		if (cbrecnext(cbp) == -1) {
			CBEPRINT;
			free(rec);
			free(key);
			return -1;
		}
	}
	free(rec);
	rec = NULL;
	free(key);
	key = NULL;

	/* give new index same lock type as rest */
	if (btlock(cbp->btpv[field], BT_UNLCK) == -1) {
		CBEPRINT;
		return -1;
	}
	if (btlock(cbp->btpv[field], ltype) == -1) {
		CBEPRINT;
		return -1;
	}

	return 0;
}
