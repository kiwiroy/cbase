/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/*man---------------------------------------------------------------------------
NAME
     lseq_.h - private header file for lseq library

SYNOPSIS
     #include "lseq_.h"

DESCRIPTION
     This is a private header file included by the lseq library
     modules.

------------------------------------------------------------------------------*/
#ifndef H_LSEQ_			/* prevent multiple includes */
#define H_LSEQ_

/* #ident	"@(#)lseq_.h	1.5 - 91/09/23" */

#include <ansi.h>

/*#define DEBUG		/* switch to enable debugging for lseq library */

#include <bool.h>

/* local headers */
#include "lseq.h"

/* tables */
extern lseq_t lsb[LSOPEN_MAX];	/* lseq control structure table declaration */

/* lshdr_t bit flags */
#define LSHMOD		  (01)	/* lseq file being modified */

/* lseq_t bit flags */
#define LSOPEN		  (03)	/* open status bits */
#define LSREAD		  (01)	/* lseq is open for reading */
#define LSWRITE		  (02)	/* lseq is open for writing */
#define LSLOCKS		 (030)	/* lock status bits */
#define LSRDLCK		 (010)	/* lseq is read locked */
#define LSWRLCK		 (020)	/* lseq is write locked */
#define LSERR		(0100)	/* error has occurred on this lseq */

/* function declarations */
#ifdef AC_PROTO
int		ls_alloc(lseq_t *lsp);
void		ls_free(lseq_t *lsp);
bool		ls_valid(lseq_t *lsp);

lsrec_t *	ls_rcalloc(lseq_t *lsp);
int		ls_rccopy(lseq_t *lsp, lsrec_t *tlsrp, const lsrec_t *slsrp);
void		ls_rcfree(lsrec_t *lsrp);
int		ls_rcget(lseq_t *lsp, lspos_t lspos, lsrec_t *lsrp);
void		ls_rcinit(lseq_t *lsp, lsrec_t *lsrp);
int		ls_rcput(lseq_t *lsp, lspos_t lspos, const lsrec_t *lsrp);
int		ls_rcputf(lseq_t *lsp, lspos_t lspos, size_t offset,
			const void *buf, size_t bufsize);
#else
int		ls_alloc();
void		ls_free();
bool		ls_valid();
lsrec_t *	ls_rcalloc();
int		ls_rccopy();
void		ls_rcfree();
int		ls_rcget();
void		ls_rcinit();
int		ls_rcput();
int		ls_rcputf();
#endif	/* #ifdef AC_PROTO */

/* macros */
#define	ls_blksize(LSP)	(offsetof(lsrec_t, recbuf) + (LSP)->lshdr.recsize)

/* lseq open types */
#define LS_READ	("r")
#define LS_RDWR	("r+")

#ifdef DEBUG
#define	LSEPRINT {							\
	fprintf(stderr, "lseq error line %d of %s. errno = %d.\n",	\
		__LINE__, __FILE__, errno);				\
}
#else
#define LSEPRINT
#endif

#endif		/* #ifndef H_LSEQ_ */
