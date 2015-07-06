/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbopen.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <blkio.h>
#include <btree.h>
#include <lseq.h>

/* local headers */
#include "cbase_.h"

/* cbase control structure table definition */
cbase_t cbb[CBOPEN_MAX];

/* cbase record position comparison function */
#ifdef AC_PROTO
int cbrposcmp(const void *p1, const void *p2, size_t n)
#else
int cbrposcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	cbrpos_t rpos1 = NIL;
	cbrpos_t rpos2 = NIL;

	memcpy(&rpos1, p1, sizeof(rpos1));
	memcpy(&rpos2, p2, sizeof(rpos2));
	if (rpos1 < rpos2) {
		return -1;
	} else if (rpos1 > rpos2) {
		return 1;
	}

	return 0;
}

/* btree field definition list */
static btfield_t btfldv[] = {
	{
		0,
		0,
		NULL,
		BT_FASC,
	},
	{
		0,
		sizeof(cbrpos_t),
		cbrposcmp,
		BT_FASC,
	},
};

/*man---------------------------------------------------------------------------
NAME
     cbopen - open a cbase

SYNOPSIS
     #include <cbase.h>

     cbase_t *cbopen(cbname, type, fldc, fldv)
     const char *cbname;
     char *type;
     int fldc;
     const cbfield_t fldv[];

DESCRIPTION
     The cbopen function opens the cbase named by cbname.  A pointer
     to the cbase_t control structure associated with the cbase is
     returned.

     type is a character string having one of the following values:

          "r"            open for reading
          "r+"           open for update (reading and writing)

     See cbcreate for explanation of the field count fldc and the
     field definition list fldv.

     cbopen will fail if one or more of the following is true:

     [EINVAL]       cbname is the NULL pointer.
     [EINVAL]       type is not "r" or "r+".
     [EINVAL]       fldc is less than 1.
     [EINVAL]       fldv is the NULL pointer.
     [EINVAL]       fldv contains an invalid field definition.
     [CBECORRUPT]   A file in the named cbase is corrupt.
     [CBEMFILE]     Too many open cbases.  The maximum is defined as
                    CBOPEN_MAX in <cbase.h>.

SEE ALSO
     cbclose, cbcreate.

DIAGNOSTICS
     On failure cbopen returns a NULL pointer, and is errno set to
     indicate the error.

NOTES
     The same field count and field definition list with which the
     cbase was created must be used each time the cbase is opened.
     Otherwise the results are undefined.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
cbase_t *cbopen(const char *cbname, const char *type, int fldc, const cbfield_t fldv[])
#else
cbase_t *cbopen(cbname, type, fldc, fldv)
const char *cbname;
const char *type;
int fldc;
const cbfield_t fldv[];
#endif
{
	int	terrno	= 0;
	cbase_t *cbp	= NULL;
	int	i	= 0;

	/* validate arguments */
	if (cbname == NULL || type == NULL || fldc < 1 || fldv == NULL) {
		errno = EINVAL;
		return NULL;
	}

	/* find free slot in cbb table */
	for (cbp = cbb; cbp < (cbb + CBOPEN_MAX); ++cbp) {
		if (!(cbp->flags & CBOPEN)) {
			break;		/* found */
		}
	}
	if (cbp >= cbb + CBOPEN_MAX) {
		errno = CBEMFILE;
		return NULL;		/* no free slots */
	}

	/* open record file */
	if (strcmp(type, CB_READ) == 0) {
		cbp->flags = CBREAD;
	} else if (strcmp(type, CB_RDWR) == 0) {
		cbp->flags = CBREAD | CBWRITE;
	} else {
		errno = EINVAL;
		return NULL;
	}
	cbp->lsp = lsopen(cbname, type);
	if (cbp->lsp == NULL) {
		if (errno == LSECORRUPT) errno = CBECORRUPT;
		if (errno != ENOENT && errno != CBECORRUPT) CBEPRINT;
		memset(cbp, 0, sizeof(*cbb));
		cbp->flags = 0;
		return NULL;
	}

	/* validate arguments */
	if (!cb_fvalid(UINT_MAX, fldc, fldv)) {
		lsclose(cbp->lsp);
		memset(cbp, 0, sizeof(*cbb));
		cbp->flags = 0;
		errno = EINVAL;
		return NULL;
	}

	/* copy field definitions into cbase structure */
	cbp->fldc = fldc;
	cbp->fldv = NULL;
	cbp->btpv = NULL;
	if (cb_alloc(cbp) == -1) {
		terrno = errno;
		lsclose(cbp->lsp);
		memset(cbp, 0, sizeof(*cbb));
		cbp->flags = 0;
		errno = terrno;
		return NULL;
	}
	memcpy(cbp->fldv, fldv, cbp->fldc * sizeof(*cbp->fldv));

	/* open key files */
	for (i = 0; i < cbp->fldc; ++i) {
		if (cbp->fldv[i].flags & CB_FKEY) {
			btfldv[1].offset = btfldv[0].len = cbp->fldv[i].len;
			btfldv[0].cmp = cbcmpv[cbp->fldv[i].type];
			cbp->btpv[i] = btopen(cbp->fldv[i].filename, type, 2, btfldv);
			if (cbp->btpv[i] == NULL) {
				if (errno != ENOENT && errno != BTECORRUPT) CBEPRINT;
				if (errno == BTECORRUPT) errno = CBECORRUPT;
				terrno = errno;
				for (i--; i >= 0; i--) {
					if (cbp->fldv[i].flags & CB_FKEY) {
						btclose(cbp->btpv[i]);
					}
				}
				lsclose(cbp->lsp);
				cb_freemem(cbp);
				memset(cbp, 0, sizeof(*cbb));
				cbp->flags = 0;
				errno = terrno;
				return NULL;
			}
		}
	}

	return cbp;
}
