/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbrmndx.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif

/* library headers */
#include <btree.h>

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cbrmndx - remove cbase index

SYNOPSIS
     #include <cbase.h>

     int cbrmndx(cbp, field)
     cbase_t *cbp;
     int field;

DESCRIPTION
     The cbrmndx function deletes an existing cbase index.  The field
     whose index is to be deleted for cbp is specified by field.

     cbrmndx will fail if one or more of the following is true:

     [CBELOCK]      cbp is not write-locked.
     [ENOENT]       The named field is not a key.
     [ENOENT]       The named field's index file does not exist.
     [EINVAL]       cbp is not a valid cbase pointer.
     [EINVAL]       field is not a valid field number for cbp.

SEE ALSO
     cbcreate, cbmkndx.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cbrmndx(cbase_t *cbp, int field)
#else
int cbrmndx(cbp, field)
cbase_t *cbp;
int field;
#endif
{
	/* validate arguments */
	if (!cb_valid(cbp)) {
		errno = EINVAL;
		return -1;
	}
	if (field < 0 || field >= cbp->fldc) {
		errno = EINVAL;
		return -1;
	}

	/* check lock status */
	if (cbgetlck(cbp) != CB_WRLCK) {
		errno = CBELOCK;
		return -1;
	}

	/* check if not an index */
	if (!(cbp->fldv[field].flags & CB_FKEY)) {
		errno = ENOENT;
		return -1;
	}

	/* close btree */
	if (btclose(cbp->btpv[field]) == -1) {
		CBEPRINT;
		return -1;
	}
	cbp->btpv[field] = NULL;

	/* delete index file */
	if (remove(cbp->fldv[field].filename)) {
		CBEPRINT;
		return -1;
	}

	/* disable old index */
	cbp->fldv[field].flags &= !(CB_FFLAGS);
	cbp->fldv[field].filename = NULL;

	return 0;
}
