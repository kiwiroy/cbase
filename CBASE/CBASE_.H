/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbase_.h	1.5 - 91/09/23" */

/*man---------------------------------------------------------------------------
NAME
     cbase_.h - private header file for cbase library

SYNOPSIS
     #include "cbase_.h"

DESCRIPTION
     This is a private header file included by the cbase library
     modules.

------------------------------------------------------------------------------*/
#ifndef H_CBASE_	/* prevent multiple includes */
#define H_CBASE_

#include <ansi.h>

/*#define DEBUG		/* switch to enable debugging for cbase library */

/* ansi headers */
#include <stdio.h>

/* non-ansi headers */
#include <bool.h>

/* local headers */
#include "cbase.h"

/* constants */
#define CBTYPECNT	((size_t)27)
#define EXPFLDDLM	('|')		/* export field delimiter */
#define EXPRECDLM	('\n')		/* export record delimiter */
#define EXPESC		('\\')		/* export field escape character */
#define CB_READ		("r")		/* cbase open types */
#define CB_RDWR		("r+")

/* tables */
#ifdef AC_PROTO
extern cbase_t cbb[CBOPEN_MAX];	/* cbase control structure table declaration */
typedef int (*cbcmp_t)(const void *p1, const void *p2, size_t n);
extern const cbcmp_t cbcmpv[CBTYPECNT];	/* comparison function table decl. */
typedef int (*cbexp_t)(FILE *fp, const void *p, size_t n);
extern const cbexp_t cbexpv[CBTYPECNT];	/* export function table decl. */
typedef int (*cbimp_t)(FILE *fp, void *p, size_t n);
extern const cbimp_t cbimpv[CBTYPECNT];	/* import function table decl. */
#else
extern cbase_t cbb[CBOPEN_MAX];	/* cbase control structure table declaration */
typedef int (*cbcmp_t)();
extern cbcmp_t cbcmpv[CBTYPECNT];	/* comparison function table decl. */
typedef int (*cbexp_t)();
extern cbexp_t cbexpv[CBTYPECNT];	/* export function table decl. */
typedef int (*cbimp_t)();
extern cbimp_t cbimpv[CBTYPECNT];	/* import function table decl. */
#endif	/* #ifdef AC_PROTO */

/* cbase_t bit flags */
#define CBOPEN		  (03)	/* open status bits */
#define CBREAD		  (01)	/* cbase is open for reading */
#define CBWRITE		  (02)	/* cbase is open for writing */
#define CBLOCKS		 (030)	/* lock status bits */
#define CBRDLCK		 (010)	/* cbase is read locked */
#define CBWRLCK		 (020)	/* cbase is write locked */
#define CBERR		(0100)	/* error has occurred on this cbase */

/* function declarations */
#ifdef AC_PROTO
int	cb_alloc(cbase_t *cbp);
void	cb_freemem(cbase_t *cbp);
bool	cb_fvalid(size_t recsize, int fldc, const cbfield_t fldv[]);
bool	cb_valid(cbase_t *cbp);
#else
int	cb_alloc();
void	cb_freemem();
bool	cb_fvalid();
bool	cb_valid();
#endif	/* #ifdef AC_PROTO */

#ifdef DEBUG
#define	CBEPRINT {							\
	fprintf(stderr, "*** cbase error line %d of %s. errno = %d.\n",	\
		__LINE__, __FILE__, errno);				\
}
#else
#define CBEPRINT
#endif

#endif		/* #ifndef H_CBASE_ */
