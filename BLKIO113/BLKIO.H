/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/*man---------------------------------------------------------------------------
NAME
     blkio - block buffered input/output library

SYNOPSIS
     #include <blkio.h>

DESCRIPTION
     blkio is a buffered input/output library for structured files.
     Because structured files are primarily accessed randomly rather
     than sequentially, they are better modeled as collections of
     blocks rather than as streams of characters.  This library may be
     used with files which fit the following criteria:

          o A header of arbitrary (possibly zero) but fixed
            length appears at the beginning of the file.
          o The data following the header is arranged in
            blocks of uniform size.

     Files fitting this model are referred to in the documentation as
     block files.

     A file to be accessed with the blkio library is declared to be a
     pointer to a defined type BLKFILE.  The bopen function creates
     certain descriptive data for the file and returns a pointer to
     designate it in all further transactions.

SEE ALSO
     bclose, bcloseall, bexit, bflpop, bflpush, bflush, bgetb, bgetbf,
     bgeth, bgethf, bopen, bputb, bputbf, bputh, bputhf, bsetbuf,
     bsetvbuf, bsync, lockb.

------------------------------------------------------------------------------*/
#ifndef H_BLKIO		/* prevent multiple includes */
#define H_BLKIO

/* #ident	"@(#)blkio.h	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#include <stdio.h>

/* constants */
#define BOPEN_MAX	(FOPEN_MAX > 60 ? FOPEN_MAX : 60)
					/* max # block files open at once */
#define NIL		((bpos_t)0)	/* nil file pointer */
#define NUL		('\0')		/* nul char */

/* macro for number elements in a static array */
#define nelems(v)	(sizeof((v)) / sizeof(*(v)))

/* macro for sizeof a structure member */
#define sizeofm(struct_t, member) ((size_t)(sizeof(((struct_t *)0)->member)))

/* type definitions */
typedef unsigned long	bpos_t;	/* block file position */

typedef union {			/* file desciptor type */
	char	c;		/* character file descriptor */
	short	s;		/* short int file descriptor */
	int	i;		/* int file descriptor */
} fd_t;

typedef struct {		/* block structure */
	bpos_t	bn;		/* block number */
	int	flags;		/* block status flags */
	size_t	more;		/* link to more recently accessed block */
	size_t	less;		/* link to less recently accessed block */
} block_t;

typedef struct {		/* block file control structure */
	fd_t	fd;		/* file descriptor for buffered file */
	int	flags;		/* buffer status flags */
	size_t	hdrsize;	/* size of file header */
	size_t	blksize;	/* size of blocks */
	size_t	bufcnt;		/* number blocks to buffer (0 if unbuffered) */
	bpos_t	endblk;		/* first block past end of file */
	size_t	most;		/* most recently accessed block [1..bufcnt] */
	size_t	least;		/* least recently accessed block [1..bufcnt] */
	block_t *blockp;	/* doubly linked list of blocks */
	void *	blkbuf;		/* buffer storage for header and blocks */
} BLKFILE;

/* function declarations */
#ifdef AC_PROTO
int		bclose(BLKFILE *bp);
void		bcloseall(void);
void		bexit(int status);
int		bflpop(BLKFILE *bp, bpos_t *bnp);
int		bflpush(BLKFILE *bp, const bpos_t *bnp);
int		bflush(BLKFILE *bp);
int		bgetb(BLKFILE *bp, bpos_t bn, void *buf);
int		bgetbf(BLKFILE *bp, bpos_t bn, size_t offset,
			void *buf, size_t bufsize);
int		bgeth(BLKFILE *bp, void *buf);
int		bgethf(BLKFILE *bp, size_t offset, void *buf, size_t bufsize);
BLKFILE *	bopen(const char *filename, const char *type,
			size_t hdrsize, size_t blksize, size_t bufcnt);
int		bputb(BLKFILE *bp, bpos_t bn, const void *buf);
int		bputbf(BLKFILE *bp, bpos_t bn,
			size_t offset, const void *buf, size_t bufsize);
int		bputh(BLKFILE *bp, const void *buf);
int		bputhf(BLKFILE *bp, size_t offset,
			const void *buf, size_t bufsize);
int		bsetbuf(BLKFILE *bp, void *buf);
int		bsetvbuf(BLKFILE *bp, void *buf, size_t blksize, size_t bufcnt);
int		bsync(BLKFILE *bp);
int		lockb(BLKFILE *bp, int ltype, bpos_t start, bpos_t len);
#else
int		bclose();
void		bcloseall();
void		bexit();
int		bflpop();
int		bflpush();
int		bflush();
int		bgetb();
int		bgetbf();
int		bgeth();
int		bgethf();
BLKFILE *	bopen();
int		bputb();
int		bputbf();
int		bputh();
int		bputhf();
int		bsetbuf();
int		bsetvbuf();
int		bsync();
int		lockb();
#endif	/* #ifdef AC_PROTO */

/* lock types */
#define B_UNLCK		(0)	/* unlock */
#define B_RDLCK		(1)	/* read lock */
#define B_WRLCK		(2)	/* write lock */
#define B_RDLKW		(3)	/* read lock, wait */
#define B_WRLKW		(4)	/* write lock, wait */

/* error codes */
#define BEOS		(0)		/* start of blkio error code domain */
#define BEMFILE		(BEOS - 1)	/* too many block files open */
#define BENOPEN		(BEOS - 2)	/* block file is not open */
#define BENBUF		(BEOS - 3)	/* buffering is off */
#define BEBUF		(BEOS - 4)	/* buffering is on */
#define BEBOUND		(BEOS - 5)	/* block boundary error */
#define BEEOF		(BEOS - 6)	/* past end of file */
#define BENFL		(BEOS - 7)	/* no free list */
#define BEPANIC		(BEOS - 8)	/* internal blkio error */

#endif	/* #ifndef H_BLKIO */
