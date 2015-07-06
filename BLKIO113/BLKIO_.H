/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/*man---------------------------------------------------------------------------
NAME
     blkio_.h - private header for blkio library

SYNOPSIS
     #include "blkio_.h"

DESCRIPTION
     This is a private header file included by the blkio library
     modules.

------------------------------------------------------------------------------*/
#ifndef H_BLKIO_	/* prevent multiple includes */
#define H_BLKIO_

/* #ident	"@(#)blkio_.h	1.5 - 91/09/23" */

#include <ansi.h>

/*#define DEBUG		/* switch to enable debugging for blkio library */

/* The OPSYS macro specifies the operating system.  Currently supported       */
/* systems are:                                                               */
#define OS_AMIGADOS	(1)		/* Amiga DOS - not yet implemented */
#define OS_DOS		(2)		/* DOS */
#define OS_MAC		(3)		/* Macintosh - not yet implemented */
#define OS_UNIX		(4)		/* UNIX */
#define OS_VMS		(5)		/* VMS - not yet implemented */
#define OPSYS		OS_DOS

/* The CCOM macro specifies the C compiler being used.  Currently supported   */
/* compilers are:                                                             */
#define CC_UNIX		(1)		/* UNIX C compiler */
#define CC_BC		(2)		/* Borland Turbo C or C++ */
#define CC_MSC		(3)		/* Microsoft C */
#define CC_MSQC		(4)		/* Microsoft Quick C */
#define CCOM		CC_BC

/*#define SINGLE_USER	/* switch to disable file locking system calls */

/* non-ansi headers */
#include <bool.h>

/* local headers */
#include "blkio.h"

/* tables */
extern BLKFILE biob[BOPEN_MAX];	/* BLKFILE control struct table declaration */

/* BLKFILE bit flags */
#define BIOOPEN		  (03)	/* open status bits */
#define BIOREAD		  (01)	/* block file is open for reading */
#define BIOWRITE	  (02)	/* block file is open for writing */
#define BIOUSRBUF	  (04)	/* user supplied buffer */
#define BIOERR		(0100)	/* error has occurred on this block file */

/* block_t bit flags */
#define BLKREAD		  (01)	/* block can be read */
#define BLKWRITE	  (02)	/* block needs to be written to disk */
#define BLKERR		(0100)	/* error has occurred on this block */

/* function declarations */
#ifdef AC_PROTO
int	b_alloc(BLKFILE *bp);
void	b_free(BLKFILE *bp);
int	b_get(BLKFILE *bp, size_t i);
int	b_initlist(BLKFILE *bp);
int	b_mkmru(BLKFILE *bp, size_t i);
int	b_put(BLKFILE *bp, size_t i);
bool	b_valid(const BLKFILE *bp);

int	b_uclose(BLKFILE *bp);
int	b_uendblk(BLKFILE *bp, bpos_t *endblkp);
int	b_ugetf(BLKFILE *bp, bpos_t bn, size_t offset, void *buf, size_t bufsize);
int	b_uopen(BLKFILE *bp, const char *filename, const char *type);
int	b_uputf(BLKFILE *bp, bpos_t bn, size_t offset, const void *buf, size_t bufsize);
#else
int	b_alloc();
void	b_free();
int	b_get();
int	b_initlist();
int	b_mkmru();
int	b_put();
bool	b_valid();

int	b_uclose();
int	b_uendblk();
int	b_ugetf();
int	b_uopen();
int	b_uputf();
#endif	/* #ifdef AC_PROTO */

/* macros */
#define	b_blkbuf(BP, N) ((void *)(					\
		(char *)(BP)->blkbuf +					\
		((N) == 0 ? 0 : (BP)->hdrsize + ((N) - 1) * (BP)->blksize)\
))
#define	b_blockp(BP, N) ((block_t *)(					\
		(char *)(BP)->blockp +	(N) * sizeof(block_t)		\
))

/* block file open types */
#define BF_READ		("r")
#define BF_RDWR		("r+")
#define BF_CRTR		("w+")
#define BF_CREATE	("c")

#ifdef DEBUG
#include <stdio.h>
#define	BEPRINT {							\
	fprintf(stderr, "*** blkio error line %d of %s. errno = %d.\n",	\
		__LINE__, __FILE__, errno);				\
}
#else
#define BEPRINT
#endif

#endif	/* #ifndef H_BLKIO_ */
