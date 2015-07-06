/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)rcops.c	1.5 - 91/09/23" */

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

/* local headers */
#include "lseq_.h"

/*man---------------------------------------------------------------------------
NAME
     ls_rcalloc - allocate memory for lseq record

SYNOPSIS
     #include "lseq_.h"

     lsrec_t *ls_rcalloc(lsp)
     lseq_t *lsp;

DESCRIPTION
     The ls_rcalloc function creates a record of the appropriate
     configuration for lseq lsp and initializes it.  The address of
     the record created is returned.

     ls_rcalloc will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [ENOMEM]       Not enough memory is available for
                    allocation by the calling process.
     [LSENOPEN]     lsp is not open.

SEE ALSO
     ls_rcfree, ls_rcinit.

DIAGNOSTICS
     On failure, a value of NULL is returned, and errno set to
     indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
lsrec_t *ls_rcalloc(lseq_t *lsp)
#else
lsrec_t *ls_rcalloc(lsp)
lseq_t *lsp;
#endif
{
	lsrec_t *lsrp = NULL;
#ifdef DEBUG
	/* validate arguments */
	if (!ls_valid(lsp)) {
		LSEPRINT;
		errno = EINVAL;
		return NULL;
	}

	/* check if not open */
	if (!(lsp->flags & LSOPEN)) {
		LSEPRINT;
		errno = LSENOPEN;
		return NULL;
	}
#endif
	/* allocate storage for main record structure */
	/* (calloc is used throughout to automatically set all bits 0) */
	lsrp = (lsrec_t *)calloc((size_t)1, sizeof(lsrec_t));
	if (lsrp == NULL) {
		LSEPRINT;
		errno = ENOMEM;
		return NULL;
	}
	lsrp->next = NIL;
	lsrp->prev = NIL;
	lsrp->recbuf = calloc((size_t)1, lsp->lshdr.recsize);
	if (lsrp->recbuf == NULL) {
		LSEPRINT;
		free(lsrp);
		errno = ENOMEM;
		return NULL;
	}

	return lsrp;
}

/*man---------------------------------------------------------------------------
NAME
     ls_rccopy - copy lseq record

SYNOPSIS
     #include "lseq_.h"

     int ls_rccopy(lsp, tlsrp, slsrp)
     lseq_t *lsp;
     lsrec_t *tlsrp;
     const lsrec_t *slsrp;

DESCRIPTION
     The ls_rccopy function makes an exact copy of source record slsrp
     in target record tlsrp.

     ls_rccopy will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [EINVAL]       tlsrp or slsrp is the NULL pointer.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int ls_rccopy(lseq_t *lsp, lsrec_t *tlsrp, const lsrec_t *slsrp)
#else
int ls_rccopy(lsp, tlsrp, slsrp)
lseq_t *lsp;
lsrec_t *tlsrp;
const lsrec_t *slsrp;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!ls_valid(lsp) || tlsrp == NULL || slsrp == NULL) {
		LSEPRINT;
		errno = EINVAL;
		return -1;
	}
#endif
	/* copy record slsrp into tlsrp */
	tlsrp->next = slsrp->next;
	tlsrp->prev = slsrp->prev;
	memcpy(tlsrp->recbuf, slsrp->recbuf, lsp->lshdr.recsize);

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     ls_rcfree - free memory allocated for lseq record

SYNOPSIS
     #include "lseq_.h"

     void ls_rcfree(lsrp)
     lsrec_t *lsrp;

DESCRIPTION
     The ls_rcfree function frees all memory allocated for lseq record
     lsrp.

SEE ALSO
     ls_rcalloc.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
void ls_rcfree(lsrec_t *lsrp)
#else
void ls_rcfree(lsrp)
lsrec_t *lsrp;
#endif
{
	if (lsrp != NULL) {
		if (lsrp->recbuf != NULL) {
			free(lsrp->recbuf);
			lsrp->recbuf = NULL;
		}
		free(lsrp);
	}

	return;
}

/*man---------------------------------------------------------------------------
NAME
     ls_rcget - lseq record get

SYNOPSIS
     #include "lseq_.h"

     int ls_rcget(lsp, lspos, lsrp)
     lseq_t *lsp;
     lspos_t lspos;
     lsrec_t *lsrp;

DESCRIPTION
     The ls_rcget function reads the record at position lspos into the
     record pointed to be lsrp.  The entire record is read, including
     the links.

SEE ALSO
     ls_rcput.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int ls_rcget(lseq_t *lsp, lspos_t lspos, lsrec_t *lsrp)
#else
int ls_rcget(lsp, lspos, lsrp)
lseq_t *lsp;
lspos_t lspos;
lsrec_t *lsrp;
#endif
{
	void *buf = NULL;
#ifdef DEBUG
	/* validate arguments */
	if (!ls_valid(lsp) || lsrp == NULL || lspos == NIL) {
		LSEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(lsp->flags & LSOPEN)) {
		LSEPRINT;
		errno = LSENOPEN;
		return -1;
	}
#endif
	/* read record from file */
	buf = calloc((size_t)1, ls_blksize(lsp));
	if (buf == NULL) {
		LSEPRINT;
		errno = ENOMEM;
		return -1;
	}
	if (bgetb(lsp->bp, (bpos_t)lspos, buf) == -1) {
		LSEPRINT;
		free(buf);
		return -1;
	}

	/* convert record from file format */
	memcpy(lsrp, buf, offsetof(lsrec_t, recbuf));
	memcpy(lsrp->recbuf, ((char *)buf + offsetof(lsrec_t, recbuf)), lsp->lshdr.recsize);

	/* free buffer */
	free(buf);
	buf = NULL;

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     ls_rcinit - lseq record initialize

SYNOPSIS
     #include "lseq_.h"

     void ls_rcinit(lsp, lsrp)
     lseq_t *lsp;
     lsrec_t *lsrp;

DESCRIPTION
     The ls_rcinit function initializes record lsrp.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
void ls_rcinit(lseq_t *lsp, lsrec_t *lsrp)
#else
void ls_rcinit(lsp, lsrp)
lseq_t *lsp;
lsrec_t *lsrp;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!ls_valid(lsp) || lsrp == NULL) {
		LSEPRINT;
		return;
	}
#endif
	/* initialize lsrp */
	lsrp->next = NIL;
	lsrp->prev = NIL;
	if (lsrp->recbuf == NULL) {
		LSEPRINT;
		return;
	}

	memset(lsrp->recbuf, 0, lsp->lshdr.recsize);

	return;
}

/*man---------------------------------------------------------------------------
NAME
     ls_rcput - lseq record put

SYNOPSIS
     #include "lseq_.h"

     int ls_rcput(lsp, lspos, lsrp)
     lseq_t *lsp;
     lspos_t lspos;
     const lsrec_t *lsrp;

DESCRIPTION
     The ls_rcput function writes the record pointed to by lsrp into
     record position lspos.  The entire record is written, including
     the links.

     ls_rcput will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [EINVAL]       lspos is NIL.
     [LSENOPEN]     lsp is not open.

SEE ALSO
     ls_rcget, ls_rcputf.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int ls_rcput(lseq_t *lsp, lspos_t lspos, const lsrec_t *lsrp)
#else
int ls_rcput(lsp, lspos, lsrp)
lseq_t *lsp;
lspos_t lspos;
const lsrec_t *lsrp;
#endif
{
	void *buf = NULL;
#ifdef DEBUG
	/* validate arguments */
	if (!ls_valid(lsp) || lspos == NIL || lsrp == NULL) {
		LSEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(lsp->flags & LSOPEN)) {
		LSEPRINT;
		errno = LSENOPEN;
		return -1;
	}
#endif
	/* convert record to file format */
	buf = calloc((size_t)1, ls_blksize(lsp));
	if (buf == NULL) {
		LSEPRINT;
		errno = ENOMEM;
		return -1;
	}
	memcpy(buf, lsrp, offsetof(lsrec_t, recbuf));
	memcpy(((char *)buf + offsetof(lsrec_t, recbuf)), lsrp->recbuf, lsp->lshdr.recsize);

	/* write record to file */
	if (bputb(lsp->bp, (bpos_t)lspos, buf) == -1) {
		LSEPRINT;
		free(buf);
		return -1;
	}

	/* free buffer */
	free(buf);
	buf = NULL;

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     ls_rcputf - lseq record field put

SYNOPSIS
     #include "lseq_.h"

     int ls_rcputf(lsp, lspos, offset, buf, bufsize)
     lseq_t *lsp;
     lspos_t lspos;
     size_t offset;
     const void *buf;
     size_t bufsize;

DESCRIPTION
     The ls_rcputf function writes the field pointed to by buf into
     record position lspos.  Only the field is written.

     ls_rcputf will fail if one or more of the following is true:

     [EINVAL]       lsp is not a valid lseq pointer.
     [EINVAL]       lspos is NIL.
     [LSEBOUND]
     [LSENOPEN]

SEE ALSO
     ls_rcget, ls_rcput.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int ls_rcputf(lseq_t *lsp, lspos_t lspos, size_t offset, const void *buf, size_t bufsize)
#else
int ls_rcputf(lsp, lspos, offset, buf, bufsize)
lseq_t *lsp;
lspos_t lspos;
size_t offset;
const void *buf;
size_t bufsize;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!ls_valid(lsp) || lspos == NIL || buf == NULL || bufsize < 1) {
		LSEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(lsp->flags & LSOPEN)) {
		LSEPRINT;
		errno = LSENOPEN;
		return -1;
	}

	/* check if record boundary crossed */
	if ((offset + bufsize) > lsp->lshdr.recsize) {
		LSEPRINT;
		errno = LSEBOUND;
		return -1;
	}
#endif
	/* write record to file */
	if (bputbf(lsp->bp, (bpos_t)lspos, offsetof(lsrec_t, recbuf) + offset, buf, bufsize) == -1) {
		LSEPRINT;
		return -1;
	}

	return 0;
}
