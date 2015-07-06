/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbops.c	1.5 - 91/09/23" */

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

/* non-ansi headers */
#include <bool.h>

/* library headers */
#include <blkio.h>
#include <btree.h>

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cb_alloc - allocate memory for cbase

SYNOPSIS
     #include "cbase_.h"

     int cb_alloc(cbp);
     cbase_t *cbp;

DESCRIPTION
     The cb_alloc function allocates the memory needed by cbp.  The
     memory is is initialized to all zeros.

     cb_alloc will fail if one or more of the following is true:

     [EINVAL]       cbp is not a valid cbase pointer.
     [ENOMEM]       Not enough memory is available for
                    allocation by the calling process.
     [CBENOPEN]     cbp is not open.

SEE ALSO
     cb_freemem.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cb_alloc(cbase_t *cbp)
#else
int cb_alloc(cbp)
cbase_t *cbp;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!cb_valid(cbp)) {
		CBEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(cbp->flags & CBOPEN)) {
		CBEPRINT;
		errno = CBENOPEN;
		return -1;
	}

	/* check for memory leak */
	if (cbp->fldv != NULL || cbp->btpv != NULL) {
		CBEPRINT;
		errno = CBEPANIC;
		return -1;
	}
#endif
	/* field definition array */
	cbp->fldv = (cbfield_t *)calloc((size_t)cbp->fldc, sizeof(*cbp->fldv));
	if (cbp->fldv == NULL) {
		CBEPRINT;
		errno = ENOMEM;
		return -1;
	}

	/* btree array */
	cbp->btpv = (btree_t **)calloc((size_t)cbp->fldc, sizeof(*cbp->btpv));
	if (cbp->btpv == NULL) {
		CBEPRINT;
		free(cbp->fldv);
		errno = ENOMEM;
		return -1;
	}

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     cb_freemem - free memory allocated for cbase

SYNOPSIS
     #include "cbase_.h"

     void cb_freemem(cbp)
     cbase_t *cbp;

DESCRIPTION
     The cb_freemem function frees all memory allocated for cbase cbp.
     If cbp is not a valid cbase, no action is taken.

SEE ALSO
     cb_alloc.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
void cb_freemem(cbase_t *cbp)
#else
void cb_freemem(cbp)
cbase_t *cbp;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!cb_valid(cbp)) {
		CBEPRINT;
		return;
	}
#endif
	/* free memory */
	if (cbp->fldv != NULL) {
		free(cbp->fldv);
		cbp->fldv = NULL;
	}
	if (cbp->btpv != NULL) {
		free(cbp->btpv);
		cbp->btpv = NULL;
	}

	return;
}

/*man---------------------------------------------------------------------------
NAME
     cb_fvalid - validate field definition list

SYNOPSIS
     #include "cbase_.h"

     bool cb_fvalid(recsize, fldc, fldv)
     size_t recsize;
     int fldc;
     const cbfield_t fldv[];

DESCRIPTION
     The cb_fvalid function determines if fldc and fldv constitute a
     valid field definition list for a cbase with records of size
     recsize.  If valid, then TRUE is returned.  If not, then FALSE is
     returned.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
bool cb_fvalid(size_t recsize, int fldc, const cbfield_t fldv[])
#else
bool cb_fvalid(recsize, fldc, fldv)
size_t recsize;
int fldc;
const cbfield_t fldv[];
#endif
{
	size_t	end	= 0;
	int	i	= 0;

	if (recsize < sizeof(bpos_t) || fldc < 1 || fldv == NULL) {
		return FALSE;
	}

	for (i = 0; i < fldc; ++i) {
		if (fldv[i].offset < end) {
			return FALSE;
		}
		if (fldv[i].len < 1) {
			return FALSE;
		}
		end = fldv[i].offset + fldv[i].len;
		if (end > recsize) {
			return FALSE;
		}
		if (fldv[i].type < 0 || fldv[i].type >= CBTYPECNT) {
			return FALSE;
		}
		if (fldv[i].flags & ~CB_FFLAGS) {
			return FALSE;
		}
		if (fldv[i].flags & CB_FKEY) {
			if (fldv[i].filename[0] == NUL) {
				return FALSE;
			}
		}
	}

	return TRUE;
}

/*man---------------------------------------------------------------------------
NAME
     cb_valid - validate cbase pointer

SYNOPSIS
     #include "cbase_.h"

     bool cb_valid(cbp)
     cbase_t *cbp;

DESCRIPTION
     The cb_valid function determines if cbp is a valid cbase pointer.
     If valid, then TRUE is returned.  If not, then FALSE is returned.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
bool cb_valid(cbase_t *cbp)
#else
bool cb_valid(cbp)
cbase_t *cbp;
#endif
{
	if (cbp < cbb || cbp > (cbb + CBOPEN_MAX - 1)) {
		return FALSE;
	}
	if ((((char *)cbp - (char *)cbb)) % sizeof(*cbb) != 0) {
		return FALSE;
	}

	return TRUE;
}
