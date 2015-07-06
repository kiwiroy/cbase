/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btops.c	1.5 - 91/09/23" */

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

/* local headers */
#include "btree_.h"

/*man---------------------------------------------------------------------------
NAME
     bt_alloc - allocate memory for btree

SYNOPSIS
     #include "btree_.h"

     int bt_alloc(btp);
     btree_t *btp;

DESCRIPTION
     The bt_alloc function allocates the memory needed by btp.  The
     memory is is initialized to all zeros.

     bt_alloc will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [ENOMEM]       Not enough memory is available for allocation by
                    the calling process.

SEE ALSO
     bt_free.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_alloc(btree_t *btp)
#else
int bt_alloc(btp)
btree_t *btp;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp)) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check for memory leak */
	if (btp->sp != NULL || btp->cbtnp != NULL) {
		BTEPRINT;
		errno = BTEPANIC;
		return -1;
	}
#endif
	/* search path (one extra element for when root splits) */
	btp->sp = (btpos_t *)calloc((size_t)(btp->bthdr.height + 1), sizeof(*btp->sp));
	if (btp->sp == NULL) {
		BTEPRINT;
		free(btp->fldv);
		errno = ENOMEM;
		return -1;
	}

	/* current node */
	btp->cbtnp = bt_ndalloc(btp);
	if (btp->cbtnp == NULL) {
		BTEPRINT;
		free(btp->fldv);
		free(btp->sp);
		return -1;
	}

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_blksize - btree block size

SYNOPSIS
     #include <btree.h>

     size_t bt_blksize(btp)
     btree_t *btp;

DESCRIPTION
     bt_blksize returns the size of the blocks in btree file btp.  If
     btp is not a valid open btree, the results are undefined.
     bt_blksize is a macro.

------------------------------------------------------------------------------*/
/* bt_blksize defined in btree_.h. */

/*man---------------------------------------------------------------------------
NAME
     bt_delete - delete btree key

SYNOPSIS
     #include <btree.h>

     int bt_delete(btp)
     btree_t *btp;

DESCRIPTION
     The bt_delete function deletes the current key from btree btp.
     The search path must be generated prior to calling bt_delete.
     The cursor is positioned to null.

     bt_delete will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [BTELOCK]      btree btp is not write locked.
     [BTENKEY]      The cursor is null.
     [BTENOPEN]     btree btp is not open.

SEE ALSO
     btdelete, btinsert, btsearch.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_delete(btree_t *btp)
#else
int bt_delete(btp)
btree_t *btp;
#endif
{
	btnode_t *	btnp	= NULL;		/* node receiving new key */
	bool		err	= FALSE;	/* error flag */
	btnode_t *	lbtnp	= NULL;		/* left sibling node */
	bpos_t		lsib	= NIL;		/* lsib location */
	/*bpos_t	node	= NIL;		/* node location */
	btnode_t *	pbtnp	= NULL;		/* parent node */
	int		pkn	= 0;		/* parent key number */
	bpos_t		pnode	= 0;		/* parent node location */
	btnode_t *	rbtnp	= NULL;		/* right sibling node */
	bpos_t		rsib	= NIL;		/* rsib location */
	unsigned long	spi	= 0;		/* search path index */
	int		terrno	= 0;		/* tmp errno */
	int		total	= 0;		/* total keys in node and sib */
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp)) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(btp->flags & BTOPEN)) {
		errno = BTENOPEN;
		return -1;
	}

	/* check lock */
	if (!(btp->flags & BTWRLCK)) {
		errno = BTELOCK;
		return -1;
	}

	/* check if cursor is null */
	if (btcursor(btp) == NULL) {
		errno = BTENKEY;
		return -1;
	}
#endif
	/* initialize working nodes */
	btnp = bt_ndalloc(btp);
	if (btnp == NULL) {
		BTEPRINT;
		return -1;
	}
	lbtnp = bt_ndalloc(btp);	/* left sibling */
	if (lbtnp == NULL) {
		BTEPRINT;
		terrno = errno;
		bt_ndfree(btnp);
		errno = terrno;
		return -1;
	}
	rbtnp = bt_ndalloc(btp);	/* right sibling */
	if (rbtnp == NULL) {
		BTEPRINT;
		terrno = errno;
		bt_ndfree(btnp);
		bt_ndfree(lbtnp);
		errno = terrno;
		return -1;
	}
	pbtnp = bt_ndalloc(btp);	/* parent */
	if (pbtnp == NULL) {
		BTEPRINT;
		terrno = errno;
		bt_ndfree(btnp);
		bt_ndfree(lbtnp);
		bt_ndfree(rbtnp);
		errno = terrno;
		return -1;
	}

	/* initialize btnp with current node */
	if (bt_ndcopy(btp, btnp, btp->cbtnp) == -1) {
		BTEPRINT;
		terrno = errno;
		bt_ndfree(btnp);
		bt_ndfree(lbtnp);
		bt_ndfree(rbtnp);
		bt_ndfree(pbtnp);
		errno = terrno;
		return -1;
	}

	/* delete key from node */
	if (bt_nddelkey(btp, btnp, btp->cbtpos.key) == -1) {
		BTEPRINT;
		terrno = errno;
		bt_ndfree(btnp);
		bt_ndfree(lbtnp);
		bt_ndfree(rbtnp);
		bt_ndfree(pbtnp);
		errno = terrno;
		return -1;
	}

	/* set modify bit in in-core header and write to file */
	btp->bthdr.flags |= BTHMOD;
	if (bputhf(btp->bp, sizeof(bpos_t),
				(char *)&btp->bthdr + sizeof(bpos_t),
				sizeof(bthdr_t) - sizeof(bpos_t)) == -1) {
		BTEPRINT;
		terrno = errno;
		bt_ndfree(btnp);
		bt_ndfree(lbtnp);
		bt_ndfree(rbtnp);
		bt_ndfree(pbtnp);
		errno = terrno;
		return -1;
	}
	if (bsync(btp->bp) == -1) {
		BTEPRINT;
		terrno = errno;
		bt_ndfree(btnp);
		bt_ndfree(lbtnp);
		bt_ndfree(rbtnp);
		bt_ndfree(pbtnp);
		errno = terrno;
		return -1;
	}

	/* loop from leaf node to root */
	for (spi = 0; spi < btp->bthdr.height; ++spi) {
		/* write to disk if node not too small */
/*??*/		if (btnp->n >= bt_ndmin(btp) || spi == btp->bthdr.height - 1 && btnp->n != 0) {
			if (bt_ndput(btp, btp->sp[spi].node, btnp) == -1) {
				BTEPRINT;
				err = TRUE;
				break;
			}
			break;
		}
		if (spi == btp->bthdr.height - 1) {		/* empty root */
			if (bt_shrink(btp, bt_kychildp(btnp, 0)) == -1) {
				BTEPRINT;
				err = TRUE;
				break;
			}
			break;
		}

		/* node needs more keys */
		/* try to shift keys with siblings */
		/* read in parent node */
		if (bt_ndget(btp, btp->sp[spi + 1].node, pbtnp) == -1) {
			BTEPRINT;
			err = TRUE;
			break;
		}
		/* get locations of nodes */
		/*node = btp->sp[spi].node;*/
		pnode = btp->sp[spi + 1].node;
		pkn = btp->sp[spi + 1].key - 1;
		if (pkn == 0) {
			lsib = NIL;
		} else {
			lsib = btnp->lsib;
		}
		if (pkn == pbtnp->n) {
			rsib = NIL;
		} else {
			rsib = btnp->rsib;
		}

		/* try shifting keys with right sibling */
		if (rsib != NIL) {
			if (bt_ndget(btp, rsib, rbtnp) == -1) {
				BTEPRINT;
				err = TRUE;
				break;
			}
			total = btnp->n + rbtnp->n;
			if (total >= 2 * bt_ndmin(btp)) {
				if (bt_ndshift(btp, btnp, rbtnp, pbtnp, pkn + 1, pnode) == -1) {
					BTEPRINT;
					err = TRUE;
					break;
				}
				break;
			}
		}

		/* try shifting keys with left sibling */
		if (lsib != NIL) {
			if (bt_ndget(btp, lsib, lbtnp) == -1) {
				BTEPRINT;
				err = TRUE;
				break;
			}
			total = lbtnp->n + btnp->n;
			if (total >= 2 * bt_ndmin(btp)) {
				btp->sp[spi].key = lbtnp->n + btp->sp[spi].key;
				if (bt_ndshift(btp, lbtnp, btnp, pbtnp, pkn, pnode) == -1) {
					BTEPRINT;
					err = TRUE;
					break;
				}
				break;
			}
		}

		/* try fusing with right sibling */
		if (rsib != NIL) {
			if (bt_ndfuse(btp, btnp, rbtnp, pbtnp, pkn + 1) == -1) {
				BTEPRINT;
				err = TRUE;
				break;
			}
			if (bt_ndcopy(btp, btnp, pbtnp) == -1) {
				BTEPRINT;
				err = TRUE;
				break;
			}
			continue;
		}

		/* try fusing with left sibling */
		if (lsib != NIL) {
			if (bt_ndfuse(btp, lbtnp, btnp, pbtnp, pkn) == -1) {
				BTEPRINT;
				err = TRUE;
				break;
			}
			if (bt_ndcopy(btp, btnp, pbtnp) == -1) {
				BTEPRINT;
				err = TRUE;
				break;
			}
			continue;
		}

		BTEPRINT;
		errno = BEPANIC;
		err = TRUE;
		break;
	}
	if (err) {
		BTEPRINT;
		terrno = errno;
		bt_ndfree(btnp);
		bt_ndfree(lbtnp);
		bt_ndfree(rbtnp);
		bt_ndfree(pbtnp);
		errno = terrno;
		return -1;
	}

	bt_ndfree(btnp);
	bt_ndfree(lbtnp);
	bt_ndfree(rbtnp);
	bt_ndfree(pbtnp);

	/* decrement key count in header */
	btp->bthdr.keycnt--;

	/* clear modify bit in in-core header and write to file */
	btp->bthdr.flags &= ~BTHMOD;
	if (bputhf(btp->bp, sizeof(bpos_t),
				(char *)&btp->bthdr + sizeof(bpos_t),
				sizeof(bthdr_t) - sizeof(bpos_t)) == -1) {
		BTEPRINT;
		return -1;
	}
	if (bsync(btp->bp) == -1) {
		BTEPRINT;
		return -1;
	}

	/* set cursor to null */
	btp->cbtpos.node = NIL;
	btp->cbtpos.key = 0;
	bt_ndinit(btp, btp->cbtnp);

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_free - free memory allocated for btree

SYNOPSIS
     #include "btree_.h"

     void bt_free(btp)
     btree_t *btp;

DESCRIPTION
     The bt_free function frees all memory allocated for btree btp.
     If btp is not a valid btree, no action is taken.

SEE ALSO
     bt_alloc.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
void bt_free(btree_t *btp)
#else
void bt_free(btp)
btree_t *btp;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp)) {
		BTEPRINT;
		return;
	}
#endif
	/* free memory */
	if (btp->fldv != NULL) free(btp->fldv);
	btp->fldv = NULL;
	if (btp->sp != NULL) free(btp->sp);
	btp->sp = NULL;
	bt_ndfree(btp->cbtnp);
	btp->cbtnp = NULL;

	return;
}

/*man---------------------------------------------------------------------------
NAME
     bt_fvalid - validate field definition list

SYNOPSIS
     #include "btree_.h"

     bool bt_fvalid(keysize, fldc, fldv)
     size_t keysize;
     int fldc;
     const btfield_t fldv[];

DESCRIPTION
     The bt_fvalid function determines if keysize, fldc, and fldv
     constitute a valid btree field definition list.  If valid, then
     TRUE is returned.  If not, then FALSE is returned.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
bool bt_fvalid(size_t keysize, int fldc, const btfield_t fldv[])
#else
bool bt_fvalid(keysize, fldc, fldv)
size_t keysize;
int fldc;
const btfield_t fldv[];
#endif
{
	int i = 0;

	if (keysize < 1 || fldc < 1 || fldv == NULL) {
		return FALSE;
	}

	for (i = 0; i < fldc; ++i) {
		if (fldv[i].len < 1) {
			return FALSE;
		}
		if (fldv[i].offset + fldv[i].len > keysize) {
			return FALSE;
		}
		if (fldv[i].cmp == NULL) {
			return FALSE;
		}
		if (fldv[i].flags & ~(BT_FFLAGS)) {
			return FALSE;
		}
	}

	return TRUE;
}

/*man---------------------------------------------------------------------------
NAME
     bt_grow - btree grow

SYNOPSIS
     #include "btree_.h"

     int bt_grow(btp, bttplp)
     btree_t *btp;
     const bttpl_t *bttplp;

DESCRIPTION
     The bt_grow function creates a new root for btree btp and inserts
     the (key, child) tuple pointed to by bttplp into the new root
     node.  This increases the tree height by one; it is the only way
     by which the height can increase.  If the tree was previously
     empty, the first and last key links in the in-core header are set
     to the new root.

     bt_grow will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       bttplp is the NULL pointer.
     [BTENOPEN]     btp is not open.

SEE ALSO
     bt_shrink.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_grow(btree_t *btp, const bttpl_t *bttplp)
#else
int bt_grow(btp, bttplp)
btree_t *btp;
const bttpl_t *bttplp;
#endif
{
	btnode_t *	btnp	= NULL;
	bpos_t		oldroot	= NIL;
	bpos_t		newroot	= NIL;
	btpos_t	*	sp	= NULL;
	int		terrno	= 0;
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp) || bttplp == NULL) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(btp->flags & BTOPEN)) {
		BTEPRINT;
		errno = BTENOPEN;
		return -1;
	}
#endif
	/* get node from free list */
	if (bflpop(btp->bp, &newroot) == -1) {
		BTEPRINT;
		return -1;
	}

	/* set root link in in-core header */
	oldroot = btp->bthdr.root;
	btp->bthdr.root = newroot;

	/* add element to search path */
	btp->sp[btp->bthdr.height].node = newroot;
	btp->sp[btp->bthdr.height].key = 1;
	++btp->bthdr.height;
	sp = (btpos_t *)realloc(btp->sp, (size_t)(btp->bthdr.height + 1) * sizeof(*sp));
	if (sp == NULL) {
		BTEPRINT;
		btp->bthdr.root = oldroot;
		--btp->bthdr.height;
		errno = ENOMEM;
		return -1;
	}
	btp->sp = sp;
	sp = NULL;
	btp->sp[btp->bthdr.height].node = NIL;
	btp->sp[btp->bthdr.height].key = 0;

	/* write new root to file */
	btnp = bt_ndalloc(btp);
	if (btnp == NULL) {
		BTEPRINT;
		terrno = errno;
		btp->bthdr.root = oldroot;
		--btp->bthdr.height;
		bflpush(btp->bp, &newroot);
		errno = terrno;
		return -1;
	}
	*bt_kychildp(btnp, 0) = oldroot;	/* link to old root */
	if (bt_ndinskey(btp, btnp, 1, bttplp) == -1) {
		BTEPRINT;
		terrno = errno;
		bt_ndfree(btnp);
		btp->bthdr.root = oldroot;
		--btp->bthdr.height;
		bflpush(btp->bp, &newroot);
		errno = terrno;
		return -1;
	}
	if (bt_ndput(btp, newroot, btnp) == -1) {
		BTEPRINT;
		terrno = errno;
		bt_ndfree(btnp);
		btp->bthdr.root = oldroot;
		--btp->bthdr.height;
		bflpush(btp->bp, &newroot);
		errno = terrno;
		return -1;
	}
	bt_ndfree(btnp);

	/* update first and last key links */
	if (oldroot == NIL) {
		btp->bthdr.first = btp->bthdr.last = newroot;
	}

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_search - search a btree

SYNOPSIS
     #include <btree.h>

     int bt_search(btp, buf)
     btree_t *btp;
     const void *buf;

DESCRIPTION
     The bt_search function searches the btree btp for the key pointed
     to by buf.  If it is found, the cursor is set to the location of
     the key and 1 is returned.  If it is not found, the cursor is set
     to the location at which it should be inserted and 0 is returned.
     Note that in the latter case the cursor may be positioned to an
     empty location.

     bt_search will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       buf is the NULL pointer.

DIAGNOSTICS
     Upon successful completion, a value of 1 is returned if the key
     was found or a value of 0 if it was not found.  On failure, a
     value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_search(btree_t *btp, const void *buf)
#else
int bt_search(btp, buf)
btree_t *btp;
const void *buf;
#endif
{
	int		found	= 0;		/* found flag */
	bpos_t		node	= NIL;		/* node position */
	unsigned long	spi	= 0;		/* search path index */
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp) || buf == NULL) {
		errno = EINVAL;
		return -1;
	}
#endif
	/* initialize btree structure for search */
	btp->cbtpos.node = NIL;			/* init cursor */
	btp->cbtpos.key = 0;
	btp->sp[btp->bthdr.height].node = NIL;	/* init search path head */
	btp->sp[btp->bthdr.height].key = 0;

	/* loop from root to leaf node */
	/* Note: spi is unsigned, so spi >= 0 will not terminate loop */
	spi = btp->bthdr.height;
	for (node = btp->bthdr.root; node != NIL;
			node = *bt_kychildp(btp->cbtnp, btp->sp[spi].key - 1)) {
		btp->sp[--spi].node = node;
		if (bt_ndget(btp, node, btp->cbtnp) == -1) {
			BTEPRINT;
			return -1;
		}
		found = bt_ndsearch(btp, btp->cbtnp, buf, &btp->sp[spi].key);
		if (found == -1) {
			BTEPRINT;
			return -1;
		}
		if (found == 1) {
			btp->sp[spi].key++;	/* move forward one */
		}
		if (spi == 0) {			/* leaf node */
			break;
		}
	}
	if (spi != 0) {
		BTEPRINT;	/* height value probably incorrect */
		errno = BTEPANIC;
		return -1;
	}

	/* set cursor */
	btp->cbtpos.node = btp->sp[0].node;
	btp->cbtpos.key = btp->sp[0].key - ((found == 1) ? 1 : 0);

	return (found == 1) ? 1: 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_shrink - btree shrink

SYNOPSIS
     #include "btree_.h"

     int bt_shrink(btp, newrootp)
     btree_t *btp;
     const bpos_t *newrootp;

DESCRIPTION
     The bt_shrink function makes *newrootp the new root for btree btp
     and returns the old root back to the free list.  This decreases
     the tree height by one; it is the only way in which the height
     can decrease.  If the old root node was also a leaf node, the
     first and last key links in the in-core header are set to NIL.

     bt_shrink will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [BTEPANIC]     btp is already empty.

SEE ALSO
     bt_grow.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_shrink(btree_t *btp, const bpos_t *newrootp)
#else
int bt_shrink(btp, newrootp)
btree_t *btp;
const bpos_t *newrootp;
#endif
{
	bpos_t		newroot	= NIL;
	bpos_t		oldroot	= NIL;
	btpos_t *	sp	= NULL;
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp) || newrootp == NULL) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(btp->flags & BTOPEN)) {
		BTEPRINT;
		errno = BTENOPEN;
		return -1;
	}

	/* check if tree already empty */
	if (btp->bthdr.root == NIL) {
		BTEPRINT;
		errno = BTEPANIC;
		return -1;
	}
#endif
	/* set root link in in-core header */
	oldroot = btp->bthdr.root;
	btp->bthdr.root = newroot = *newrootp;

	/* remove element from search path */
	--btp->bthdr.height;
	sp = (btpos_t *)realloc(btp->sp, (size_t)(btp->bthdr.height + 1) * sizeof(*sp));
	if (sp == NULL) {
		BTEPRINT;
		btp->bthdr.root = oldroot;
		++btp->bthdr.height;
		errno = ENOMEM;
		return -1;
	}
	btp->sp = sp;
	sp = NULL;
	btp->sp[btp->bthdr.height].node = NIL;
	btp->sp[btp->bthdr.height].key = 0;

	/* push root back onto free list stack */
	if (bflpush(btp->bp, &oldroot) == -1) {
		BTEPRINT;
		btp->bthdr.root = oldroot;
		++btp->bthdr.height;
		return -1;
	}

	/* update first and last key links */
	if (newroot == NIL) {
		btp->bthdr.first = btp->bthdr.last = NIL;
	}

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_valid - validate btree pointer

SYNOPSIS
     #include "btree_.h"

     bool bt_valid(btp)
     btree_t *btp;

DESCRIPTION
     The bt_valid function determines if btp is a valid btree pointer.
     If valid, then TRUE is returned.  If not, then FALSE is returned.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
bool bt_valid(btree_t *btp)
#else
bool bt_valid(btp)
btree_t *btp;
#endif
{
	if (btp < btb || btp > (btb + BTOPEN_MAX - 1)) {
		return FALSE;
	}
	if ((((char *)btp - (char *)btb)) % sizeof(*btb) != 0) {
		return FALSE;
	}

	return TRUE;
}
