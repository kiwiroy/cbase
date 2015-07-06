/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/*man---------------------------------------------------------------------------
NAME
     btree - B-tree file management library

SYNOPSIS
     #include <btree.h>

DESCRIPTION
     The btree library consists of a set of routines for the creation
     and manipulation of file-based B-tree structures.  The B+-tree
     variety of B-tree is used to provide efficient sequential as well
     as random access.

     btree uses the blkio library for file access and buffering.
     bexit should therefore be used in place of exit.

SEE ALSO
     btclose, btcreate, btcursor, btdelcur, btdelete, btfirst, btfix,
     btgetcur, btgetk, btgetlck, btinsert, btkeycmp, btkeycnt,
     btkeysize, btlast, btlock, btnext, btopen, btprev, btsearch,
     btsetbuf, btsetcur, btsetvbuf, btsync.

------------------------------------------------------------------------------*/
#ifndef H_BTREE		/* prevent multiple includes */
#define H_BTREE

/* #ident	"@(#)btree.h	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#ifdef AC_STDDEF
#include <stddef.h>
#endif

/* library headers */
#include <blkio.h>

/* constants */
#define BTOPEN_MAX	BOPEN_MAX	/* max # btrees open at once */
#define BTBUFCNT	((size_t)12)	/* default # of nodes to buffer */

/* macro to calculate buffer size needed by a btree */
#define	BTBUFSIZ(M, KEYSIZE, BUFCNT) ((size_t)(				\
	sizeof(bthdr_t) +						\
	(BUFCNT) * (							\
		offsetof(btnode_t, keyv) +				\
		((M) - 1) * (KEYSIZE) +					\
		(M) * sizeof(bpos_t)					\
	)								\
))

/* type definitions */
typedef struct {		/* btree position */
	bpos_t	node;		/* block number of node */
	int	key;		/* key number in node */
} btpos_t;

/* pointer to field comparison function */
#ifdef AC_PROTO
typedef int (*btcmp_t)(const void *p1, const void *p2, size_t n);
#else
typedef int (*btcmp_t)();
#endif

typedef struct {		/* btree node */
	bpos_t	lsib;		/* block number of left sibling */
	bpos_t	rsib;		/* block number of right sibling */
	int	n;		/* number keys in node  (n <= (m - 1)) */
	void *	keyv;		/* pointer to m keys */
	bpos_t *childv;		/* pointer to (m + 1) child block numbers */
} btnode_t;

typedef struct {		/* btree file header */
	bpos_t	flh;		/* position of free list head */
	int	m;		/* order of tree */
	size_t	keysize;	/* key size */
	int	flags;		/* flags */
	bpos_t	root;		/* position of root node */
	bpos_t	first;		/* position of first leaf node */
	bpos_t	last;		/* position of last leaf node */
	unsigned long keycnt;	/* number keys currently in btree */
	unsigned long height;	/* current height of btree */
} bthdr_t;

typedef struct {		/* field definition */
	size_t	offset;		/* offset of field in key */
	size_t	len;		/* length of field */
	btcmp_t	cmp;		/* comparison function */
	int	flags;		/* flags */
} btfield_t;

typedef struct {		/* btree control structure */
	bthdr_t	bthdr;		/* header record */
	BLKFILE *bp;		/* block file */
	int	flags;		/* status flags */
	int	fldc;		/* number of fields */
	btfield_t *fldv;	/* field definitions */
	btpos_t	cbtpos;		/* current btree position */
	btnode_t *cbtnp;	/* current node */
	btpos_t *sp;		/* search path to current position */
} btree_t;

/* btfield_t bit flags */
#define BT_FFLAGS	(3)		/* mask for all flags */
#define BT_FASC		(1)		/* ascending order */
#define BT_FDSC		(2)		/* descending order */

/* function declarations */
#ifdef AC_PROTO
int		btclose(btree_t *btp);
int		btcreate(const char *filename, int m, size_t keysize,
			int fldc, const btfield_t fldv[]);
int		btdelcur(btree_t *btp);
int		btdelete(btree_t *btp, const void *buf);
int		btfirst(btree_t *btp);
int		btfix(const char *filename, int m, size_t keysize,
			int fldc, const btfield_t fldv[]);
int		btgetcur(btree_t *btp, btpos_t *btposp);
int		btgetk(btree_t *btp, void *buf);
int		btgetlck(btree_t *btp);
int		btinsert(btree_t *btp, const void *buf);
int		btkeycmp(btree_t *btp, const void *buf1, const void *buf2);
int		btlast(btree_t *btp);
int		btlock(btree_t *btp, int ltype);
int		btnext(btree_t *btp);
btree_t *	btopen(const char *filename, const char *type,
			int fldc, const btfield_t fldv[]);
int		btprev(btree_t *btp);
int		btsearch(btree_t *btp, const void *buf);
int		btsetbuf(btree_t *btp, void *buf);
int		btsetcur(btree_t *btp, const btpos_t *btposp);
int		btsetvbuf(btree_t *btp, void *buf, size_t bufcnt);
int		btsync(btree_t *btp);
#else
int		btclose();
int		btcreate();
int		btdelcur();
int		btdelete();
int		btfirst();
int		btfix();
int		btgetcur();
int		btgetk();
int		btgetlck();
int		btinsert();
int		btkeycmp();
int		btlast();
int		btlock();
int		btnext();
btree_t *	btopen();
int		btprev();
int		btsearch();
int		btsetbuf();
int		btsetcur();
int		btsetvbuf();
int		btsync();
#endif	/* #ifdef AC_PROTO */

/* macros */
#define	btcursor(BTP) ((void *)(					\
		(BTP)->cbtpos.node == NIL ? NULL : ((char *)NULL + 1)	\
))
#define	btkeycnt(BTP)	((BTP)->bthdr.keycnt)
#define	btkeysize(BTP)	((BTP)->bthdr.keysize)

/* lock types */
#define BT_UNLCK	(0)	/* unlock */
#define BT_RDLCK	(1)	/* read lock */
#define BT_WRLCK	(2)	/* write lock */
#define BT_RDLKW	(3)	/* read lock, wait */
#define BT_WRLKW	(4)	/* write lock, wait */

/* error codes */
#define BTEOS		(-40)		/* start of btree error code domain */
#define BTEMFILE	(BTEOS - 1)	/* too many open btrees */
#define BTECORRUPT	(BTEOS - 2)	/* btree is corrupt */
#define BTENOPEN	(BTEOS - 3)	/* btree is not open */
#define BTENBUF		(BTEOS - 4)	/* buffering is off */
#define BTELOCK		(BTEOS - 5)	/* incorrect lock type */
#define BTENKEY		(BTEOS - 6)	/* no key */
#define BTEDUP		(BTEOS - 7)	/* duplicate key */
#define BTEEOF		(BTEOS - 8)	/* past end of file */
#define BTEPANIC	(BTEOS - 9)	/* internal btree error */

#endif		/* #ifndef BTREE_H */
