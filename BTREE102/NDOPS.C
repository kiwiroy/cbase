/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)ndops.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
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
     bt_ndalloc - allocate memory for node

SYNOPSIS
     #include "btree_.h"

     btnode_t *bt_ndalloc(btp)
     btree_t *btp;

DESCRIPTION
     The bt_ndalloc function allocates an initialized in-core node for
     btree btp.  The address of the node created is returned.

     bt_ndalloc will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [ENOMEM]       Not enough memory is available for allocation by
                    the calling process.
     [BTENOPEN]     btp is not open.

SEE ALSO
     bt_ndfree, bt_ndinit.

DIAGNOSTICS
     On failure, a NULL pointer is returned, and errno set to indicate
     the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
btnode_t *bt_ndalloc(btree_t *btp)
#else
btnode_t *bt_ndalloc(btp)
btree_t *btp;
#endif
{
	btnode_t *btnp = NULL;
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp)) {
		BTEPRINT;
		errno = EINVAL;
		return NULL;
	}

	/* check if not open */
	if (!(btp->flags & BTOPEN)) {
		BTEPRINT;
		errno = BTENOPEN;
		return NULL;
	}
#endif
	/* allocate storage for main node structure */
	/* (calloc is used throughout to automatically set all bits 0) */
	btnp = (btnode_t *)calloc((size_t)1, sizeof(btnode_t));
	if (btnp == NULL) {
		BTEPRINT;
		errno = ENOMEM;
		return NULL;
	}
	btnp->lsib = NIL;
	btnp->rsib = NIL;
	btnp->n = 0;
	/* key array [1..m] (extra slot is for overflow) */
	btnp->keyv = calloc((size_t)btp->bthdr.m, btp->bthdr.keysize);
	if (btnp->keyv == NULL) {
		BTEPRINT;
		free(btnp);
		errno = ENOMEM;
		return NULL;
	}
	/* child node file postion array [0..m] */
	btnp->childv = (bpos_t *)calloc((size_t)(btp->bthdr.m + 1), sizeof(*btnp->childv));
	if (btnp->childv == NULL) {
		BTEPRINT;
		free(btnp->keyv);
		free(btnp);
		errno = ENOMEM;
		return NULL;
	}

	return btnp;
}

/*man---------------------------------------------------------------------------
NAME
     bt_ndcopy - copy btree node

SYNOPSIS
     #include "btree_.h"

     int bt_ndcopy(btp, tbtnp, sbtnp)
     btree_t *btp;
     btnode_t *tbtnp;
     const btnode_t *sbtnp;

DESCRIPTION
     The bt_ndcopy function makes an exact copy of the in-core node
     pointed to by sbtnp to the in-core node pointed to by tbtnp.

     bt_ndcopy will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       tbtnp or sbtnp is the NULL pointer.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_ndcopy(btree_t *btp, btnode_t *tbtnp, const btnode_t *sbtnp)
#else
int bt_ndcopy(btp, tbtnp, sbtnp)
btree_t *btp;
btnode_t *tbtnp;
const btnode_t *sbtnp;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp) || tbtnp == NULL || sbtnp == NULL) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}
#endif
	/* copy node sbtnp into tbtnp */
	tbtnp->lsib = sbtnp->lsib;
	tbtnp->rsib = sbtnp->rsib;
	tbtnp->n = sbtnp->n;
	memcpy(bt_kykeyp(btp, tbtnp, 1), bt_kykeyp(btp, sbtnp, 1), (size_t)(btp->bthdr.m * btp->bthdr.keysize));
	memcpy(bt_kychildp(tbtnp, 0), bt_kychildp(sbtnp, 0), (size_t)((btp->bthdr.m + 1) * sizeof(*bt_kychildp(tbtnp, 0))));

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_nddelkey - delete key from btree node

SYNOPSIS
     #include "btree_.h"

     int bt_nddelkey(btp, btnp, kn)
     btree_t *btp;
     btnode_t *btnp;
     int kn;

DESCRIPTION
     The bt_nddelkey function deletes the tuple kn from the in-core
     node pointed to by btnp.

     bt_nddelkey will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       btnp is the NULL pointer.

SEE ALSO
     bt_ndinskey, bt_ndsearch.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_nddelkey(btree_t *btp, btnode_t *btnp, int kn)
#else
int bt_nddelkey(btp, btnp, kn)
btree_t *btp;
btnode_t *btnp;
int kn;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp) || btnp == NULL) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}
#endif
	/* delete key kn */
	if (bt_kyshift(btp, btnp, kn + 1, -1) == -1) {
		BTEPRINT;
		return -1;
	}

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_ndfree - free in-core node

SYNOPSIS
     #include "btree_.h"

     void bt_ndfree(btnp)
     btnode_t *btnp;

DESCRIPTION
     The bt_ndfree function frees the in-core node pointed to by btnp.

SEE ALSO
     bt_ndalloc.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
void bt_ndfree(btnode_t *btnp)
#else
void bt_ndfree(btnp)
btnode_t *btnp;
#endif
{
	if (btnp == NULL) {
		return;
	}
	if (btnp->keyv != NULL) free(btnp->keyv);
	btnp->keyv = NULL;
	if (btnp->childv != NULL) free(btnp->childv);
	btnp->childv = NULL;
	free(btnp);

	return;
}

/*man---------------------------------------------------------------------------
NAME
     bt_ndfuse - btree node fuse

SYNOPSIS
     #include "btree_.h"

     int bt_ndfuse(btp, lbtnp, rbtnp, pbtnp, pkn)
     btree_t *btp;
     btnode_t *lbtnp;
     btnode_t *rbtnp;
     btnode_t *pbtnp;
     int pkn;

DESCRIPTION
     The bt_ndfuse function fuses two nodes.  lbtnp and rbtnp point to
     in-core copies of nodes to be fused, the left and right sibling,
     respectively.  pbtnp points to an in-core copy of the parent
     node.  pkn is the number of the key in pbtnp whose left child is
     lbtnp and right child is rbtnp.

     On return, lbtnp contains the fused node and rbtnp is empty.
     Both the left and right nodes are written to the file.  pbtnp is
     modified during the fusion, but is not written to the file; the
     calling program must write pbtnp to the file.

     bt_ndfuse will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       btnp, rbtnp, or pbtnp is NULL.
     [EINVAL]       pkn is less than 1 or greater than pbtnp->n.
     [BTENOPEN]     btp is not open.
     [BTEPANIC]     The total number of keys in lbtnp and rbtnp is
                    too large to fuse into one node.

SEE ALSO
     bt_ndshift, bt_ndsplit.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_ndfuse(btree_t *btp, btnode_t *lbtnp, btnode_t *rbtnp, btnode_t *pbtnp, int pkn)
#else
int bt_ndfuse(btp, lbtnp, rbtnp, pbtnp, pkn)
btree_t *btp;
btnode_t *lbtnp;
btnode_t *rbtnp;
btnode_t *pbtnp;
int pkn;
#endif
{
	bttpl_t	bttpl;			/* (key, child address) tuple */
	bool	leaf	= FALSE;	/* leaf node flag */
	bpos_t	lnode	= 0;		/* left node block number */
	bpos_t	rnode	= 0;		/* right node block number */

	/* initialize automatic aggregates */
	memset(&bttpl, 0, sizeof(bttpl));
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp)) {
		BTEPRINT;
		errno = EINVAL;
		return NULL;
	}
	if (lbtnp == NULL || rbtnp == NULL || pbtnp == NUL) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}
	if (pkn < 1 || pkn > pbtnp->n) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if too many keys for fusion */
	if (lbtnp->n + rbtnp->n > bt_ndmax(btp) - (leaf ? 0 : 1)) {
		BTEPRINT;
		errno = BTEPANIC;
		return -1;
	}
#endif
	/* check if leaf */
	if (bt_ndleaf(lbtnp)) {
		leaf = TRUE;
	}

	/* get block number of sibling nodes */
	lnode = rbtnp->lsib;
	rnode = lbtnp->rsib;

	/* if internal node, bring down parent key pkn */
	if (!leaf) {
		bttpl.keyp = calloc((size_t)1, btp->bthdr.keysize);
		if (bttpl.keyp == NULL) {
			BTEPRINT;
			errno = ENOMEM;
			return -1;
		}
		if (bt_kyread(btp, pbtnp, pkn, &bttpl) == -1) {
			BTEPRINT;
			free(bttpl.keyp);
			return -1;
		}
		bttpl.child = *bt_kychildp(rbtnp, 0);
		if (bt_ndinskey(btp, lbtnp, lbtnp->n + 1, &bttpl) == -1) {
			BTEPRINT;
			free(bttpl.keyp);
			return -1;
		}
		free(bttpl.keyp);
		bttpl.keyp = NULL;
	}

	/* move keys from rbtnp to lbtnp */
	if (bt_kymvleft(btp, lbtnp, rbtnp, rbtnp->n) == -1) {
		BTEPRINT;
		return -1;
	}

	/* return right node to free list */
	if (bflpush(btp->bp, &rnode) == -1) {
		BTEPRINT;
		return -1;
	}

	/* fix sibling links */		/*L=left N=new right */
	lbtnp->rsib = rbtnp->rsib;	/* L -> N */
	if (lbtnp->rsib == NIL) {	/* L <- N */
		if (leaf) btp->bthdr.last = lnode;
	} else {
		if (bputbf(btp->bp, lbtnp->rsib, offsetof(btnode_t, lsib), &lnode, sizeof(lbtnp->lsib)) == -1) {
			BTEPRINT;
			return -1;
		}
	}

	/* initialize in-core copy of right sibling */
	bt_ndinit(btp, rbtnp);

	/* delete parent key of right sibling */
	if (bt_nddelkey(btp, pbtnp, pkn) == -1) {
		BTEPRINT;
		return -1;
	}

	/* write fused node */
	if (bt_ndput(btp, lnode, lbtnp) == -1) {
		BTEPRINT;
		return -1;
	}

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_ndget - get btree node from file

SYNOPSIS
     #include "btree_.h"

     int bt_ndget(btp, node, btnp)
     btree_t *btp;
     bpos_t node;
     btnode_t *btnp;

DESCRIPTION
     The bt_ndget function reads the contents of a node into the
     in-core node pointed to by btnp to the file.  node is the block
     number of the node in the file.

     bt_ndget will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       btnp is NULL.
     [EINVAL]       node is NIL.
     [EINVAL]       btnp is NULL.
     [BTENOPEN]     btp is not open.

SEE ALSO
     bt_ndput.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_ndget(btree_t *btp, bpos_t node, btnode_t *btnp)
#else
int bt_ndget(btp, node, btnp)
btree_t *btp;
bpos_t node;
btnode_t *btnp;
#endif
{
	void *buf = NULL;
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp) || node == NIL || btnp == NULL) {
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
	/* read node from file */
	buf = calloc((size_t)1, bt_blksize(btp));
	if (buf == NULL) {
		BTEPRINT;
		errno = ENOMEM;
		return -1;
	}
	if (bgetb(btp->bp, node, buf) == -1) {
		BTEPRINT;
		free(buf);
		return -1;
	}

	/* convert file node to in-core format */
	memcpy(btnp, buf, offsetof(btnode_t, keyv));
	memcpy(btnp->keyv,
		((char *)buf + offsetof(btnode_t, keyv)),
		((btp->bthdr.m - 1) * btp->bthdr.keysize));
	memcpy(btnp->childv,
		((char *)buf + offsetof(btnode_t, keyv) +
			((btp->bthdr.m - 1) * btp->bthdr.keysize)),
		(btp->bthdr.m * sizeof(*btnp->childv)));

	/* free buffer */
	free(buf);

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_ndinit - initialize in-core node

SYNOPSIS
     #include "btree_.h"

     void bt_ndinit(btp, btnp)
     btree_t *btp;
     btnode_t *btnp;

DESCRIPTION
     The bt_ndinit function initializes the in-core node btnp.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
void bt_ndinit(btree_t *btp, btnode_t *btnp)
#else
void bt_ndinit(btp, btnp)
btree_t *btp;
btnode_t *btnp;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp) || btnp == NULL) {
		BTEPRINT;
		errno = EINVAL;
		return;
	}
#endif
	/* initialize btnp */
	btnp->lsib = NIL;
	btnp->rsib = NIL;
	btnp->n = 0;
	memset(btnp->keyv, 0, (size_t)(btp->bthdr.m * btp->bthdr.keysize));
	memset(btnp->childv, 0, (size_t)((btp->bthdr.m + 1) * sizeof(*btnp->childv)));

	return;
}

/*man---------------------------------------------------------------------------
NAME
     bt_ndinskey - insert key into btree node

SYNOPSIS
     #include "btree_.h"

     int bt_ndinskey(btp, btnp, kn, bttplp)
     btree_t *btp;
     btnode_t *btnp;
     int kn;
     const bttpl_t *bttplp;

DESCRIPTION
     The bt_ndinskey function inserts bttplp into the knth slot in
     the in-core btree node btnp.  The node is not written to the
     file.

     bt_ndinskey will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       btnp is the NULL pointer.
     [EINVAL]       kn is less than 1 or greater than the number of
                    keys in btnp plus 1.

SEE ALSO
     bt_nddelkey, bt_ndsearch.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_ndinskey(btree_t *btp, btnode_t *btnp, int kn, const bttpl_t *bttplp)
#else
int bt_ndinskey(btp, btnp, kn, bttplp)
btree_t *btp;
btnode_t *btnp;
int kn;
const bttpl_t *bttplp;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp) || btnp == NULL || bttplp == NULL) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}
	if (kn < 1 || kn > btnp->n + 1) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if room to insert */
	if (btnp->n >= btp->bthdr.m) {
		BTEPRINT;
		errno = BTEPANIC;
		return -1;
	}
#endif
	/* make an empty slot */
	if (bt_kyshift(btp, btnp, kn, 1) == -1) {
		BTEPRINT;
		return -1;
	}

	/* put new key into empty slot */
	if (bt_kywrite(btp, btnp, kn, bttplp) == -1) {
		BTEPRINT;
		return -1;
	}

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_ndleaf - is btree node leaf

SYNOPSIS
     #include "btree_.h"

     int bt_ndleaf(btnp)
     btnode_t *btnp;

DESCRIPTION
     bt_ndleaf returns a true value if btnp is a leaf node or a false
     value if it is not.  If btnp does not point to a valid in-core
     btree node, the results are undefined.  bt_ndleaf is a macro.

SEE ALSO
     bt_ndmax, bt_ndmin.

------------------------------------------------------------------------------*/
/* bt_ndleaf is defined in btree_.h. */

/*man---------------------------------------------------------------------------
NAME
     bt_ndmax - maximum keys per node

SYNOPSIS
     #include "btree_.h"

     int bt_ndmax(btp)
     btree_t *btp;

DESCRIPTION
     bt_ndmin returns the maximum number of keys allowable in a node
     of btree btp.  If btp does not point to a valid open btree, the
     results are undefined.  bt_ndmax is a macro.

SEE ALSO
     bt_ndleaf, bt_ndmin.

------------------------------------------------------------------------------*/
/* bt_ndmax is defined in btree_.h. */

/*man---------------------------------------------------------------------------
NAME
     bt_ndmin - minimum keys per node

SYNOPSIS
     #include "btree_.h"

     int bt_ndmin(btp)
     btree_t *btp;

DESCRIPTION
     bt_ndmin returns the minimum number of keys allowable in a node
     of btree btp.  If btp does not point to a valid open btree, the
     results are undefined.  bt_ndmin is a macro.

SEE ALSO
     bt_ndleaf, bt_ndmax.

------------------------------------------------------------------------------*/
/* bt_ndmin is defined in btree_.h. */

/*man---------------------------------------------------------------------------
NAME
     bt_ndput - put btree node to file

SYNOPSIS
     #include "btree_.h"

     int bt_ndput(btp, node, btnp)
     btree_t *btp;
     bpos_t node;
     const btnode_t *btnp;

DESCRIPTION
     The bt_ndput function writes the contents of the in-core node
     pointed to by btnp to the file.  node is the block number of the
     node in the file.

     bt_ndput will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       btnp is NULL.
     [EINVAL]       node is NIL.
     [EINVAL]       btnp is NULL.
     [BTENOPEN]     btp is not open.

SEE ALSO
     bt_ndget.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_ndput(btree_t *btp, bpos_t node, const btnode_t *btnp)
#else
int bt_ndput(btp, node, btnp)
btree_t *btp;
bpos_t node;
const btnode_t *btnp;
#endif
{
	void *buf = NULL;
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp) || node == NIL || btnp == NULL) {
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
	/* convert in-core node to file format */
	buf = calloc((size_t)1, bt_blksize(btp));
	if (buf == NULL) {
		BTEPRINT;
		errno = ENOMEM;
		return -1;
	}
	memcpy(buf, btnp, offsetof(btnode_t, keyv));
	memcpy(((char *)buf + offsetof(btnode_t, keyv)),
		btnp->keyv,
		((btp->bthdr.m - 1) * btp->bthdr.keysize));
	memcpy(((char *)buf + offsetof(btnode_t, keyv) +
			((btp->bthdr.m - 1) * btp->bthdr.keysize)),
		btnp->childv,
		(btp->bthdr.m * sizeof(*btnp->childv)));

	/* write node to file */
	if (bputb(btp->bp, node, buf) == -1) {
		BTEPRINT;
		free(buf);
		return -1;
	}

	/* free buffer */
	free(buf);

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_ndsearch - search btree node

SYNOPSIS
     #include "btree_.h"

     int bt_ndsearch(btp, btnp, buf, knp)
     btree_t *btp;
     const btnode_t *btnp;
     const void *buf;
     int *knp;

DESCRIPTION
     Function searches the in-core node btnp for the key pointed to by
     buf.  On return, the location of the smallest key >= that pointed
     to by buf is in the location pointed to by knp.

     bt_ndsearch will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       btnp is NULL.
     [EINVAL]       buf is NULL.
     [EINVAL]       knp is NULL.

SEE ALSO
     bt_nddelkey, bt_ndinskey.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned if the key
     was found or a value of 1 is it was not found.  Otherwise, a
     value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_ndsearch(btree_t *btp, const btnode_t *btnp, const void *buf, int *knp)
#else
int bt_ndsearch(btp, btnp, buf, knp)
btree_t *btp;
const btnode_t *btnp;
const void *buf;
int *knp;
#endif
{
	int	cmp	= 0;		/* result of key comparison */
	int	fld	= 0;		/* field number */
	bool	found	= FALSE;	/* key found flag */
	int	kn	= 0;		/* key number */
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp) || btnp == NULL || buf == NULL || knp == NULL) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}
#endif
	/* initialize */
	*knp = 0;

	/* locate key */
	for (kn = 1; kn <= btnp->n; ++kn) {
		for (fld = 0; fld < btp->fldc; ++fld) {
			cmp = (*btp->fldv[fld].cmp)(bt_kykfp(btp, btnp, kn, fld),
					(char *)buf + btp->fldv[fld].offset,
					btp->fldv[fld].len);
			if (cmp != 0) {
				break;
			}
		}
		if (cmp == 0) {
			found = TRUE;
			break;
		}
		if (btp->fldv[fld].flags & BT_FDSC) {
			if (cmp < 0) break;	/* descending order */
		} else {
			if (cmp > 0) break;	/* ascending order */
		}
	}
	*knp = kn;

	return found ? 1 : 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_ndshift - node shift

SYNOPSIS
     #include "btree_.h"

     int bt_ndshift(btp, lbtnp, rbtnp, pbtnp, pkn, pnode)
     btree_t *btp;
     btnode_t *lbtnp;
     btnode_t *rbtnp;
     btnode_t *pbtnp;
     int pkn;
     bpos_t pnode;

DESCRIPTION
     The bt_ndshift function shifts keys between two sibling nodes to
     give them both the same number of keys (+/- 1).  lbtnp and rbtnp
     are in-core copies of the two siblings.  pbtnp is an in-core copy
     of the parent node of the two siblings.  pkn is the number of the
     key in pbtnp whose left child is lbtnp and right child is rbtnp.

     The left, right, and parent nodes are written to the file.

     bt_ndshift will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       lbtnp, rbtnp, or pbtnp is NULL.
     [EINVAL]       pkn is less than 1 or greater than pbtnp->n.
     [BTENOPEN]     btp is not open.
     [BTEPANIC]     Not enough keys in lbtnp and rbtnp to achieve
                    the minimum number in both nodes.

SEE ALSO
     bt_ndfuse, bt_ndsplit.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_ndshift(btree_t *btp, btnode_t *lbtnp, btnode_t *rbtnp, btnode_t *pbtnp, int pkn, bpos_t pnode)
#else
int bt_ndshift(btp, lbtnp, rbtnp, pbtnp, pkn, pnode)
btree_t *btp;
btnode_t *lbtnp;
btnode_t *rbtnp;
btnode_t *pbtnp;
int pkn;
bpos_t pnode;
#endif
{
	bttpl_t	bttpl;			/* (key, child address) tuple */
	bool	leaf	= FALSE;	/* leaf node flag */
	bool	right	= FALSE;	/* shift direction flag */
	int	ns	= 0;		/* number keys to shift */

	/* initialize automatic aggregates */
	memset(&bttpl, 0, sizeof(bttpl));
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp)) {
		BTEPRINT;
		errno = EINVAL;
		return NULL;
	}
	if (lbtnp == NULL || rbtnp == NULL || pbtnp == NULL) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}
	if (pkn < 1 || pkn > pbtnp->n) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(btp->flags & BTOPEN)) {
		BTEPRINT;
		errno = BTENOPEN;
		return NULL;
	}

	/* check if enough keys to shift */
if (lbtnp->n + rbtnp->n < 2 * bt_ndmin(btp)) {
		errno = BTEPANIC;
		return -1;
	}

	/* check if too many keys to shift */
	if (lbtnp->n + rbtnp->n > 2 * bt_ndmax(btp)) {
		errno = BTEPANIC;
		return -1;
	}
#endif
/*printf("IN NDSHIFT: pkn = %d, pnode = %lu.\n", pkn, pnode);*/
	/* set leaf and direction flags */
	if (bt_ndleaf(lbtnp)) {
		leaf = TRUE;
	}
	if (lbtnp->n > rbtnp->n) {
		right = TRUE;
	} else {
		right = FALSE;
	}

	/* if internal node, bring down parent key pkn */
	if (!leaf) {
		bttpl.keyp = calloc((size_t)1, btp->bthdr.keysize);
		if (bttpl.keyp == NULL) {
			BTEPRINT;
			errno = ENOMEM;
			return -1;
		}
		if (bt_kyread(btp, pbtnp, pkn, &bttpl) == -1) {
			BTEPRINT;
			free(bttpl.keyp);
			return -1;
		}
		bttpl.child = *bt_kychildp(rbtnp, 0);
		if (right) {
			if (bt_ndinskey(btp, rbtnp, 1, &bttpl) == -1) {
				BTEPRINT;
				free(bttpl.keyp);
				return -1;
			}
			*bt_kychildp(rbtnp, 0) = *bt_kychildp(lbtnp, lbtnp->n);
		} else {
			if (bt_ndinskey(btp, lbtnp, lbtnp->n + 1, &bttpl) == -1) {
				BTEPRINT;
				free(bttpl.keyp);
				return -1;
			}
			*bt_kychildp(lbtnp, lbtnp->n) = *bt_kychildp(rbtnp, 0);
		}
		free(bttpl.keyp);
		bttpl.keyp = NULL;
	}

	/* calculate number of keys to shift (+ -> shift to right) */
	ns = (lbtnp->n - rbtnp->n) / 2;
	if (ns < 0) ns = -ns;

	/* shift keys */
	if (right) {
		if (bt_kymvright(btp, lbtnp, rbtnp, ns) == -1) {
			BTEPRINT;
			return -1;
		}
	} else {
		if (bt_kymvleft(btp, lbtnp, rbtnp, ns) == -1) {
			BTEPRINT;
			return -1;
		}
	}

	/* bring up new parent key (child of pkn remains right node) */
	if (!leaf) {
		if (lbtnp->n > rbtnp->n) {
			memcpy(bt_kykeyp(btp, pbtnp, pkn), bt_kykeyp(btp, lbtnp, lbtnp->n), btp->bthdr.keysize);
			if (bt_nddelkey(btp, lbtnp, lbtnp->n) == -1) {
				BTEPRINT;
				return -1;
			}
		} else {
			memcpy(bt_kykeyp(btp, pbtnp, pkn), bt_kykeyp(btp, rbtnp, 1), btp->bthdr.keysize);
			*bt_kychildp(rbtnp, 0) = *bt_kychildp(rbtnp, 1);
			if (bt_nddelkey(btp, rbtnp, 1) == -1) {
				BTEPRINT;
				return -1;
			}
		}
	} else {
		memcpy(bt_kykeyp(btp, pbtnp, pkn), bt_kykeyp(btp, rbtnp, 1), btp->bthdr.keysize);
	}

	/* write lbtnp, rbtnp, and pbtnp to file */
	if (bt_ndput(btp, *bt_kychildp(pbtnp, pkn - 1), lbtnp) == -1) {
		BTEPRINT;
		return -1;
	}
	if (bt_ndput(btp, *bt_kychildp(pbtnp, pkn), rbtnp) == -1) {
		BTEPRINT;
		return -1;
	}
	if (bt_ndput(btp, pnode, pbtnp) == -1) {
		BTEPRINT;
		return -1;
	}

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_ndsplit - btree node split

SYNOPSIS
     #include "btree_.h"

     int bt_ndsplit(btp, node, btnp, rbtnp, bttplp)
     btree_t *btp;
     bpos_t node;
     btnode_t *btnp;
     btnode_t *rbtnp;
     bttpl_t *bttplp;

DESCRIPTION
     The bt_ndsplit function splits a btree node.  btnp points to an
     in-core copy of the node to be split.  node is the block number
     of this node in the file.  The splitting will produce a new right
     sibling for this node.  Both the modified and the new node are
     written to the file.  If btnp had a right sibling prior to the
     split, the lsib link field in that node is updated, also.

     On return, btnp and rbtnp contain copies of the split node and
     its new right sibling, respectively, and the tuple pointed to by
     bttplp contains the (key, child address) tuple to be inserted in
     the parent of the split node; bttplp->child contains the block
     number of the new right sibling node.

     bt_ndsplit will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       btnp, rbtnp, or bttplp is NULL.
     [BTENOPEN]     btp is not open.
     [BTEPANIC]     The number of keys in btnp is not equal to m.

SEE ALSO
     bt_ndfuse, bt_ndshift.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_ndsplit(btree_t *btp, bpos_t node, btnode_t *btnp, btnode_t *rbtnp, bttpl_t *bttplp)
#else
int bt_ndsplit(btp, node, btnp, rbtnp, bttplp)
btree_t *btp;
bpos_t node;
btnode_t *btnp;
btnode_t *rbtnp;
bttpl_t *bttplp;
#endif
{
	bool	leaf	= FALSE;	/* leaf node flag */
	int	midkn	= 0;		/* middle key number */
	bpos_t	rnode	= 0;		/* right node block number */
	int	terrno	= 0;		/* tmp errno */
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp)) {
		BTEPRINT;
		errno = EINVAL;
		return NULL;
	}
	if (btnp == NULL || rbtnp == NULL || bttplp == NULL) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(btp->flags & BTOPEN)) {
		BTEPRINT;
		errno = BTENOPEN;
		return NULL;
	}

	/* check if node not one over full */
	if (btnp->n != bt_ndmax(btp) + 1) {
		BTEPRINT;
		errno = BTEPANIC;
		return -1;
	}
#endif
	/* initialize */
	bt_ndinit(btp, rbtnp);

	/* check if leaf node */
	if (bt_ndleaf(btnp)) {
		leaf = TRUE;
	}

	/* get new block for right sibling node */
	if (bflpop(btp->bp, &rnode) == -1) {
		BTEPRINT;
		return -1;
	}

	/* fix sibling links */		/*L=left R=right O=old right */
	rbtnp->rsib = btnp->rsib;	/* L    R -> O */
	btnp->rsib = rnode;		/* L -> R    O */
	rbtnp->lsib = node;		/* L <- R    O */
	if (rbtnp->rsib == NIL) {	/* L    R <- O */
		if (leaf) btp->bthdr.last = rnode;
	} else {
		if (bputbf(btp->bp, rbtnp->rsib, offsetof(btnode_t, lsib),
					&rnode, sizeof(rbtnp->lsib)) == -1) {
			BTEPRINT;
			terrno = errno;
			bflpush(btp->bp, &rnode);
			errno = terrno;
			return -1;
		}
	}

	/* calculate middle key number */
	midkn = btnp->n / 2 + 1;

	/* get middle (key, child) tuple into bttplp */
	if (bt_kyread(btp, btnp, midkn, bttplp) == -1) {
		BTEPRINT;
		terrno = errno;
		bflpush(btp->bp, &rnode);
		errno = terrno;
		return -1;
	}
	bttplp->child = rnode;

	/* shift keys from left sibling */
	if (leaf) {
		/* move midkn thru n to right sibling */
		if (bt_kymvright(btp, btnp, rbtnp, btnp->n - midkn + 1) == -1) {
			BTEPRINT;
			return -1;
		}
	} else {
		/* move midkn + 1 thru n to right sibling */
		if (bt_kymvright(btp, btnp, rbtnp, btnp->n - midkn) == -1) {
			BTEPRINT;
			return -1;
		}
		/* delete midkn */
		if (bt_nddelkey(btp, btnp, midkn) == -1) {
			BTEPRINT;
			return -1;
		}
	}

	/* write split nodes */
	if (bt_ndput(btp, node, btnp) == -1) {
		BTEPRINT;
		return -1;
	}
	if (bt_ndput(btp, rnode, rbtnp) == -1) {
		BTEPRINT;
		return -1;
	}

	return 0;
}
