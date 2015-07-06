/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/*man---------------------------------------------------------------------------
NAME
     lseq - linked sequential file management library

SYNOPSIS
     #include <lseq.h>

DESCRIPTION
     The lseq library consists of a set of routines for the creation
     and manipulation of doubly-linked sequential files.

     lseq uses the blkio library for file access and buffering.
     Therefore bexit should be used in place of exit when using lseq.

SEE ALSO
     lsclose, lscreate, lscursor, lsdelcur, lsfirst, lsgetcur,
     lsgetlck, lsgetr, lsgetrf, lsinsert, lslast, lslock, lsnext,
     lsopen, lsprev, lsputr, lsputrf, lsreccnt, lsrecsize, lssearch,
     lssetbuf, lssetcur, lssetvbuf, lssync.

------------------------------------------------------------------------------*/
#ifndef H_LSEQ		/* prevent multiple includes */
#define H_LSEQ

/* #ident	"@(#)lseq.h	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#ifdef AC_STDDEF
#include <stddef.h>
#endif

/* library headers */
#include <blkio.h>

/* constants */
#define LSOPEN_MAX	BOPEN_MAX	/* max # lseqs open at once */
#define LSBUFCNT	((size_t)10)	/* default # of records to buffer */

/* macro to calculate buffer size needed by an lseq */
#define LSBUFSIZ(RECSIZE, BUFCNT) ((size_t)(				\
	sizeof(lshdr_t) +						\
	(BUFCNT) * (offsetof(lsrec_t, recbuf) + (RECSIZE)		\
))

/* type definitions */
typedef bpos_t	lspos_t;	/* lseq position */
#ifdef AC_PROTO
typedef int (*lscmp_t)(const void *p1, const void *p2, size_t n);
#else
typedef int (*lscmp_t)();
#endif

typedef struct {		/* lseq record */
	lspos_t	next;		/* next record in list */
	lspos_t	prev;		/* previous record in list */
	void *	recbuf;		/* record contents */
} lsrec_t;

typedef struct {		/* lseq file header */
	bpos_t	flh;		/* position of free list head */
	size_t	recsize;	/* record size */
	int	flags;		/* flags */
	lspos_t	first;		/* position of first record */
	lspos_t	last;		/* position of last record */
	unsigned long reccnt;	/* number records currently in lseq */
} lshdr_t;

typedef struct {		/* lseq control structure */
	lshdr_t	lshdr;		/* file header */
	BLKFILE *bp;		/* block file */
	int	flags;		/* status flags */
	lspos_t	clspos;		/* current lseq position */
	lsrec_t *clsrp;		/* current record */
} lseq_t;

/* function declarations */
#ifdef AC_PROTO
int		lsclose(lseq_t *lsp);
int		lscreate(const char *filename, size_t recsize);
int		lsdelcur(lseq_t *lsp);
int		lsfirst(lseq_t *lsp);
int		lsgetcur(lseq_t *lsp, lspos_t *lsposp);
int		lsgetlck(lseq_t *lsp);
int		lsgetr(lseq_t *lsp, void *buf);
int		lsgetrf(lseq_t *lsp, size_t offset, void *buf, size_t bufsize);
int		lsinsert(lseq_t *lsp, const void *buf);
int		lslast(lseq_t *lsp);
int		lslock(lseq_t *lsp, int ltype);
int		lsnext(lseq_t *lsp);
lseq_t *	lsopen(const char *filename, const char *type);
int		lsprev(lseq_t *lsp);
int		lsputr(lseq_t *lsp, const void *buf);
int		lsputrf(lseq_t *lsp, size_t offset, const void *buf,
			size_t bufsize);
int		lssearch(lseq_t *lsp, size_t offset, const void *buf,
			size_t bufsize, lscmp_t cmp);
int		lssetbuf(lseq_t *lsp, void *buf);
int		lssetcur(lseq_t *lsp, const lspos_t *lsposp);
int		lssetvbuf(lseq_t *lsp, void *buf, size_t bufcnt);
int		lssync(lseq_t *lsp);
#else
int		lsclose();
int		lscreate();
int		lsdelcur();
int		lsfirst();
int		lsgetcur();
int		lsgetlck();
int		lsgetr();
int		lsgetrf();
int		lsinsert();
int		lslast();
int		lslock();
int		lsnext();
lseq_t *	lsopen();
int		lsprev();
int		lsputr();
int		lsputrf();
int		lssearch();
int		lssetbuf();
int		lssetcur();
int		lssetvbuf();
int		lssync();
#endif	/* #ifdef AC_PROTO */

/* macros */
#define	lscursor(LSP)	((void *)(					\
	(LSP)->clspos == NIL ? NULL : ((char *)NULL + 1)		\
))
#define	lsreccnt(LSP)	((LSP)->lshdr.reccnt)
#define	lsrecsize(LSP)	((LSP)->lshdr.recsize)

/* lock types */
#define LS_UNLCK	(0)	/* unlock */
#define LS_RDLCK	(1)	/* read lock */
#define LS_WRLCK	(2)	/* write lock */
#define LS_RDLKW	(3)	/* read lock, wait */
#define LS_WRLKW	(4)	/* write lock, wait */

/* lseq error codes */
#define LSEOS		(-20)	/* start of lseq error code domain */
#define LSEMFILE	(LSEOS - 1)	/* too many open lseqs */
#define LSECORRUPT	(LSEOS - 2)	/* lseq is corrupt */
#define LSENOPEN	(LSEOS - 3)	/* lseq not open */
#define LSENBUF		(LSEOS - 4)	/* buffering is off */
#define LSELOCK		(LSEOS - 5)	/* incorrect lock type */
#define LSENREC		(LSEOS - 6)	/* no record */
#define LSEBOUND	(LSEOS - 7)	/* record boundary error */
#define LSEEOF		(LSEOS - 8)	/* past end of file */
#define LSEPANIC	(LSEOS - 9)	/* internal lseq error */

#endif		/* #ifndef H_LSEQ */
