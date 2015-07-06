/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/*man---------------------------------------------------------------------------
NAME
     btree_.h - private header file for btree library

SYNOPSIS
     #include "btree_.h"

DESCRIPTION
     This is a private header file included by the btree library
     modules.  It contains function declarations and macros for
     internal use by the btree library.

------------------------------------------------------------------------------*/
#ifndef H_BTREE_	/* prevent multiple includes */
#define H_BTREE_

/* #ident	"@(#)btree_.h	1.5 - 91/09/23" */

#include <ansi.h>

/*#define DEBUG		/* switch to enable debugging for btree library */

/* ansi headers */
#ifdef AC_STRING
#include <string.h>
#endif

#include <bool.h>

/* local headers */
#include "btree.h"

/* type definitions */
typedef struct {		/* (key, child) tuple */
	void *	keyp;		/* pointer to single key */
	bpos_t	child;		/* child link */
} bttpl_t;

/* tables */
extern btree_t btb[BTOPEN_MAX];	/* btree control structure table declaration */

/* bthdr_t bit flags */
#define BTHMOD		  (01)	/* btree file being modified */

/* btree_t bit flags */
#define BTOPEN		  (03)	/* open status bits */
#define BTREAD		  (01)	/* btree is open for reading */
#define BTWRITE		  (02)	/* btree is open for writing */
#define BTLOCKS		 (030)	/* lock status bits */
#define BTRDLCK		 (010)	/* btree is read locked */
#define BTWRLCK		 (020)	/* btree is write locked */
#define BTERR		(0100)	/* error has occurred on this btree */

/* function declarations */
#ifdef AC_PROTO
int		bt_alloc(btree_t *btp);
int		bt_delete(btree_t *btp);
void		bt_free(btree_t *btp);
bool		bt_fvalid(size_t keysize, int fldc, const btfield_t fldv[]);
int		bt_grow(btree_t *btp, const bttpl_t *bttplp);
int		bt_search(btree_t *btp, const void *buf);
int		bt_shrink(btree_t *btp, const bpos_t *newrootp);
bool		bt_valid(btree_t *btp);

btnode_t *	bt_ndalloc(btree_t *btp);
int		bt_ndcopy(btree_t *btp, btnode_t *tbtnp, const btnode_t *sbtnp);
int		bt_nddelkey(btree_t *btp, btnode_t *btnp, int kn);
void		bt_ndfree(btnode_t *btnp);
int		bt_ndfuse(btree_t *btp, btnode_t *lbtnp, btnode_t *rbtnp,
			btnode_t *pbtnp, int pkn);
int		bt_ndget(btree_t *btp, bpos_t node, btnode_t *btnp);
void		bt_ndinit(btree_t *btp, btnode_t *btnp);
int		bt_ndinskey(btree_t *btp, btnode_t *btnp, int kn,
			const bttpl_t *bttplp);
int		bt_ndput(btree_t *btp, bpos_t node, const btnode_t *btnp);
int		bt_ndsearch(btree_t *btp, const btnode_t *btnp, const void *buf,
			int *knp);
int		bt_ndshift(btree_t *btp, btnode_t *lbtnp, btnode_t *rbtnp,
			btnode_t *pbtnp, int pkn, bpos_t pnode);
int		bt_ndsplit(btree_t *btp, bpos_t node, btnode_t *btnp,
			btnode_t *rbtnp, bttpl_t *bttplp);

int		bt_kymvleft(btree_t *btp, btnode_t *lbtnp, btnode_t *rbtnp,
			int nm);
int		bt_kymvright(btree_t *btp, btnode_t *lbtnp, btnode_t *rbtnp,
			int nm);
int		bt_kyread(btree_t *btp, const btnode_t *btnp, int kn,
			bttpl_t *bttplp);
int		bt_kyshift(btree_t *btp, btnode_t *btnp, int kn, int ns);
int		bt_kywrite(btree_t *btp, btnode_t *btnp, int kn,
			const bttpl_t *bttplp);

void		bt_dgbtp(btree_t *btp, FILE *fp);
int		bt_dgbtree(btree_t *btp, bpos_t node, FILE *fp);
void		bt_dgnode(btree_t *btp, btnode_t *btnp, FILE *fp);
void		bt_dgtuple(btree_t *btp, bttpl_t *bttplp, FILE *fp);
#else
int		bt_alloc();
int		bt_delete();
void		bt_free();
bool		bt_fvalid();
int		bt_grow();
int		bt_search();
int		bt_shrink();
bool		bt_valid();
btnode_t *	bt_ndalloc();
int		bt_ndcopy();
int		bt_nddelkey();
void		bt_ndfree();
int		bt_ndfuse();
int		bt_ndget();
void		bt_ndinit();
int		bt_ndinskey();
int		bt_ndput();
int		bt_ndsearch();
int		bt_ndshift();
int		bt_ndsplit();
int		bt_kymvleft();
int		bt_kymvright();
int		bt_kyread();
int		bt_kyshift();
int		bt_kywrite();
void		bt_dgbtp();
int		bt_dgbtree();
void		bt_dgnode();
void		bt_dgtuple();
#endif	/* #ifdef AC_PROTO */

/* macros */
#define bt_blksize(BTP) ((size_t)(					\
	offsetof(btnode_t, keyv) +					\
	((BTP)->bthdr.m - 1) * (BTP)->bthdr.keysize +			\
	(BTP)->bthdr.m * sizeof(bpos_t)					\
))
#define	bt_ndleaf(BTNP)	(*bt_kychildp(BTNP, 0) == NIL)
#define	bt_ndmax(BTP)	((int)((BTP)->bthdr.m - 1))
#define	bt_ndmin(BTP)	((int)((((BTP)->bthdr.m + 1) / 2) - 1))
#define bt_kychildp(BTNP, N) ((bpos_t *)(				\
			(char *)(BTNP)->childv +			\
			(size_t)(N) * sizeof(bpos_t)			\
))
#define bt_kykeyp(BTP, BTNP, N) ((void *)(				\
			(char *)(BTNP)->keyv +				\
			((size_t)(N) - 1) * (BTP)->bthdr.keysize	\
))
#define bt_kykfp(BTP, BTNP, N, F) ((void *)(				\
			(char *)bt_kykeyp((BTP), (BTNP), (N)) +		\
			(BTP)->fldv[(F)].offset				\
))

/* btree open types */
#define BT_READ		("r")
#define BT_RDWR		("r+")

#ifdef DEBUG
#define	BTEPRINT {							\
	fprintf(stderr, "*** btree error line %d of %s.  errno = %d.\n",\
		__LINE__, __FILE__, errno);				\
}
#else
#define BTEPRINT
#endif

#endif		/* #ifndef H_BTREE_ */
