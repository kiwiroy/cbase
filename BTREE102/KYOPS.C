/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)kyops.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <blkio.h>

/* local headers */
#include "btree_.h"

/*man---------------------------------------------------------------------------
NAME
     bt_kychildp - btree node child

SYNOPSIS
     #include "btree_.h"

     bpos_t *bt_kychildp(btnp, kn)
     btnode_t *btnp;
     int kn;

DESCRIPTION
     bt_kychildp returns a pointer to the knth child in node btnp.
     If btnp is not a valid btree node or if kn < 0 or
     kn > btnp->n + 1 the results are undefined.  bt_kychildp is a macro.

SEE ALSO
     bt_kykeyp.

------------------------------------------------------------------------------*/
/* bt_kychildp is defined in btree_.h. */

/*man---------------------------------------------------------------------------
NAME
     bt_kykeyp - btree node key

SYNOPSIS
     #include "btree_.h"

     void *bt_kykeyp(btnp, kn)
     btnode_t *btnp;
     int kn;

DESCRIPTION
     bt_kykeyp returns a pointer to the knth key in node btnp.  If
     btnp is not a valid btree node or if kn < 1 or kn > btnp->n + 1
     the results are undefined.  bt_kychildp is a macro.

SEE ALSO
     bt_kychildp.

------------------------------------------------------------------------------*/
/* bt_kykeyp is defined in btree_.h. */

/*man---------------------------------------------------------------------------
NAME
     bt_kykfp - btree node key field

SYNOPSIS
     #include "btree_.h"

     void *bt_kykfp(btp, btnp, kn, fn)
     btree_t *btp;
     btnode_t *btnp;
     int kn;
     int fn;

DESCRIPTION
     bt_kykfp returns a pointer to the fnth field of the knth key in
     node btnp.  If btnp is not a valid btree node or if kn < 1 or
     kn > btnp->n + 1 the results are undefined.  bt_kychildp is a
     macro.

SEE ALSO
     bt_kychildp.

------------------------------------------------------------------------------*/
/* bt_kykfp is defined in btree_.h. */

/*man---------------------------------------------------------------------------
NAME
     bt_kymvleft - move keys left

SYNOPSIS
     #include "btree_.h"

     int bt_kymvleft(btp, lbtnp, rbtnp, nm)
     btree_t *btp;
     btnode_t *lbtnp;
     btnode_t *rbtnp;
     int nm;

DESCRIPTION
     Function moves the leftmost nm tuples and child 0 (keys 1..nm and
     children 0..nm) from node rbtnp to lbtnp.  They are appended
     after the last key in lbtnp, its rightmost child being
     overwritten.  The key count in each node is corrected for the
     shift.  No other changes are made.

     bt_kymvleft will fail if one or more of the following is true: 

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       lbtnp or rbtnp is NULL. 
     [EINVAL]       lbtnp and rbtnp point to the same node.
     [EINVAL]       rbtnp has less than nm keys.
     [EINVAL]       lbtnp does not have room for nm keys.

SEE ALSO
     bt_kymvright, bt_kyshift.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_kymvleft(btree_t *btp, btnode_t *lbtnp, btnode_t *rbtnp, int nm)
#else
int bt_kymvleft(btp, lbtnp, rbtnp, nm)
btree_t *btp;
btnode_t *lbtnp;
btnode_t *rbtnp;
int nm;
#endif
{
	int	ks = 0;		/* first key to copy */
	int	ke = 0;		/* last key to copy */
	int	kt = 0;		/* target key */
	void *	ps = NULL;	/* pointer to copy source */
	void *	pe = NULL;	/* pointer to copy source end */
	void *	pt = NULL;	/* pointer to copy target */
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp) || lbtnp == NULL || rbtnp == NULL || lbtnp == rbtnp) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}
	if (nm > rbtnp->n || nm + lbtnp->n > bt_ndmax(btp) + 1) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}
#endif
	/* move bttpls */
	ks = 1;
	ke = nm;
	kt = lbtnp->n + 1;
	ps = bt_kykeyp(btp, rbtnp, ks);
	pe = bt_kykeyp(btp, rbtnp, ke + 1);
	pt = bt_kykeyp(btp, lbtnp, kt);
	memcpy(pt, ps, (size_t)((char *)pe - (char *)ps));
	ps = bt_kychildp(rbtnp, ks - 1);
	pe = bt_kychildp(rbtnp, ke + 1);
	pt = bt_kychildp(lbtnp, kt - 1);
	memcpy(pt, ps, (size_t)((char *)pe - (char *)ps));

	/* adjust key count of left node */
	lbtnp->n += nm;

	/* delete keys from rbtnp */
	ks = nm + 1;
	ke = rbtnp->n;
	kt = 1;
	ps = bt_kykeyp(btp, rbtnp, ks);
	pe = bt_kykeyp(btp, rbtnp, ke + 1);
	pt = bt_kykeyp(btp, rbtnp, kt);
	memmove(pt, ps, (size_t)((char *)pe - (char *)ps));
	ps = bt_kychildp(rbtnp, ks - 1);
	pe = bt_kychildp(rbtnp, ke + 1);
	pt = bt_kychildp(rbtnp, kt - 1);
	memmove(pt, ps, (size_t)((char *)pe - (char *)ps));

	/* adjust key count of right node */
	rbtnp->n -= nm;
	if (rbtnp->n < btp->bthdr.m) {
		ps = bt_kykeyp(btp, rbtnp, rbtnp->n + 1);
		pe = bt_kykeyp(btp, rbtnp, btp->bthdr.m + 1);
		memset(ps, 0, (size_t)((char *)pe - (char *)ps));
		ps = bt_kychildp(rbtnp, rbtnp->n + 1);
		pe = bt_kychildp(rbtnp, btp->bthdr.m + 1);
		memset(ps, 0, (size_t)((char *)pe - (char *)ps));
	}

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_kymvright - move keys right

SYNOPSIS
     #include "btree_.h"

     int bt_kymvright(btp, lbtnp, rbtnp, nm)
     btree_t *btp;
     btnode_t *lbtnp;
     btnode_t *rbtnp;
     int nm;

DESCRIPTION
     Function moves the rightmost nm bttpls and the child preceding
     them (keys (n + 1 - nm)..n and children (n - nm)..n) from node
     lbtnp to rbtnp.  They are inserted before the first key in rbtnp,
     its leftmost child being overwritten.  The key count in each node
     is corrected for the shift.  No other changes are made.

     bt_kymvright will fail if one or more of the following is true: 

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       lbtnp or rbtnp is NULL. 
     [EINVAL]       lbtnp and rbtnp are same node.
     [EINVAL]       rbtnp does not have nm keys.
     [EINVAL]       lbtnp does not have room for nm keys.

SEE ALSO
     bt_kymvleft, bt_kyshift.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_kymvright(btree_t *btp, btnode_t *lbtnp, btnode_t *rbtnp, int nm)
#else
int bt_kymvright(btp, lbtnp, rbtnp, nm)
btree_t *btp;
btnode_t *lbtnp;
btnode_t *rbtnp;
int nm;
#endif
{
	int	ks = 0;		/* first key to copy */
	int	ke = 0;		/* last key to copy */
	int	kt = 0;		/* target key */
	void *	ps = NULL;	/* pointer to copy source */
	void *	pe = NULL;	/* pointer to copy source end */
	void *	pt = NULL;	/* pointer to copy target */
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp) || lbtnp == NULL || rbtnp == NULL || lbtnp == rbtnp) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}
	if (nm > lbtnp->n || nm + rbtnp->n > bt_ndmax(btp) + 1) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}
#endif
	/* make room in right node */
	ks = 1;
	ke = rbtnp->n;
	kt = nm + 1;
	ps = bt_kykeyp(btp, rbtnp, ks);
	pe = bt_kykeyp(btp, rbtnp, ke + 1);
	pt = bt_kykeyp(btp, rbtnp, kt);
	memmove(pt, ps, (size_t)((char *)pe - (char *)ps));
	ps = bt_kychildp(rbtnp, ks - 1);
	pe = bt_kychildp(rbtnp, ke + 1);
	pt = bt_kychildp(rbtnp, kt - 1);
	memmove(pt, ps, (size_t)((char *)pe - (char *)ps));

	/* adjust key count of right node */
	rbtnp->n += nm;

	/* move bttpls */
	ks = lbtnp->n - nm + 1;
	ke = lbtnp->n;
	kt = 1;
	ps = bt_kykeyp(btp, lbtnp, ks);
	pe = bt_kykeyp(btp, lbtnp, ke + 1);
	pt = bt_kykeyp(btp, rbtnp, kt);
	memcpy(pt, ps, (size_t)((char *)pe - (char *)ps));
	memset(ps, 0, (size_t)((char *)pe - (char *)ps));
	ps = bt_kychildp(lbtnp, ks - 1);
	pe = bt_kychildp(lbtnp, ke + 1);
	pt = bt_kychildp(rbtnp, kt - 1);
	memcpy(pt, ps, (size_t)((char *)pe - (char *)ps));
	ps = bt_kychildp(lbtnp, ks);
	memset(ps, 0, (size_t)((char *)pe - (char *)ps));

	/* adjust key count of left node */
	lbtnp->n -= nm;

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_kyread - read key

SYNOPSIS
     #include "btree_.h"

     int bt_kyread(btp, btnp, kn, bttplp)
     btree_t *btp;
     btnode_t *btnp;
     int kn;
     bttpl_t *bttplp;

DESCRIPTION
     Function reads the (key, child) bttpl in slot kn of node btnp.
     The results are returned in bttplp.  If kn is zero, then the key
     field of bttplp is cleared and child 0 is written in the child
     field.

     bt_kyread will fail if one or more of the following is true:

     [EINVAL]       btp, btnp, or bttplp is NULL. 
     [EINVAL]       btnp contains fewer than kn keys.
     [EINVAL]       kn < 0 or kn > btnp->n.

SEE ALSO
     bt_kywrite.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_kyread(btree_t *btp, const btnode_t *btnp, int kn, bttpl_t *bttplp)
#else
int bt_kyread(btp, btnp, kn, bttplp)
btree_t *btp;
const btnode_t *btnp;
int kn;
bttpl_t *bttplp;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp) || btnp == NULL || bttplp == NULL) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}
	if (kn < 0 || kn > btnp->n) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}
#endif
	if (kn == 0) {
		memset(bttplp->keyp, 0, sizeof(bttplp->keyp));
	} else {
		memcpy(bttplp->keyp, bt_kykeyp(btp, btnp, kn), btp->bthdr.keysize);
	}
	bttplp->child = *bt_kychildp(btnp, kn);

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_kyshift - shift keys

SYNOPSIS
     #include "btree_.h"

     int bt_kyshift(btp, btnp, kn, ns)
     btree_t *btp;
     btnode_t *btnp;
     int kn;
     int ns;

DESCRIPTION
     Function shifts bttpls kn and above in node btnp by ns slots.  If
     ns > 0, the keys are shifted up.  If ns < 0, they are shifted
     down.  The key count in is corrected for the shift.  No other
     changes are made.

     bt_kymvleft will fail if one or more of the following is true: 

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       btnp is NULL. 
     [EINVAL]       kn < 1 or kn > btnp->n + 1.

SEE ALSO
     bt_kymvleft, bt_kymvright.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_kyshift(btree_t *btp, btnode_t *btnp, int kn, int ns)
#else
int bt_kyshift(btp, btnp, kn, ns)
btree_t *btp;
btnode_t *btnp;
int kn;
int ns;
#endif
{
	int	ks = 0;		/* first key to copy */
	int	ke = 0;		/* last key to copy */
	int	kt = 0;		/* target key */
	void *	ps = NULL;	/* pointer to copy source */
	void *	pe = NULL;	/* pointer to copy source end */
	void *	pt = NULL;	/* pointer to copy target */
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp) || btnp == NULL) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}
	if (kn < 1 || kn > btnp->n + 1) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}
#endif
	if (((int)btnp->n + ns) > (int)btp->bthdr.m) {/* keys shifted out top */
		BTEPRINT;
		errno = BTEPANIC;
		return -1;
	}
	if (-ns >= (int)kn) {			/* keys shifted out bottom */
		BTEPRINT;
		errno = BTEPANIC;
		return -1;
	}

	/* shift keys */
	ks = kn;
	ke = btnp->n;
	kt = kn + ns;
	ps = bt_kykeyp(btp, btnp, ks);
	pe = bt_kykeyp(btp, btnp, ke + 1);
	pt = bt_kykeyp(btp, btnp, kt);
	memmove(pt, ps, (size_t)((char *)pe - (char *)ps));
	ps = bt_kychildp(btnp, ks);
	pe = bt_kychildp(btnp, ke + 1);
	pt = bt_kychildp(btnp, kt);
	memmove(pt, ps, (size_t)((char *)pe - (char *)ps));

	/* adjust number of keys */
	btnp->n = (int)btnp->n + ns;

	/* clear memory above last key */
	if (ns < 0) {
		ps = bt_kykeyp(btp, btnp, btnp->n + 1);
		pe = bt_kykeyp(btp, btnp, btp->bthdr.m + 1);
		memset(ps, 0, (size_t)((char *)pe - (char *)ps));
		ps = bt_kychildp(btnp, btnp->n + 1);
		pe = bt_kychildp(btnp, btp->bthdr.m + 1);
		memset(ps, 0, (size_t)((char *)pe - (char *)ps));
	}

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_kywrite - write key

SYNOPSIS
     #include "btree_.h"

     int bt_kywrite(btp, btnp, kn, bttplp)
     btree_t *btp;
     btnode_t *btnp;
     int kn;
     const bttpl_t *bttplp;

DESCRIPTION
     Function writes the (key, child) bttpl contained in bttplp in
     slot kn of node btnp.  If kn is zero, then the contents of the
     key field of bttplp are ignored and the contents of the child
     field are written to child 0 of btnp.  If bttplp is NULL, the
     slot is cleared.

     bt_kywrite will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EIVNAL]       btnp is NULL. 
     [EINVAL]       btnp contains fewer than kn keys.

SEE ALSO
     bt_kywrite.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_kywrite(btree_t *btp, btnode_t *btnp, int kn, const bttpl_t *bttplp)
#else
int bt_kywrite(btp, btnp, kn, bttplp)
btree_t *btp;
btnode_t *btnp;
int kn;
const bttpl_t *bttplp;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!bt_valid(btp) || btnp == NULL) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}
	if (kn < 0 || kn > btnp->n) {
		BTEPRINT;
		errno = EINVAL;
		return -1;
	}
#endif
	if (bttplp == NULL) {
		if (kn != 0) {
			memset(bt_kykeyp(btp, btnp, kn), 0, btp->bthdr.keysize);
		}
		*bt_kychildp(btnp, kn) = NIL;
	} else {
		if (kn != 0) {
			memcpy(bt_kykeyp(btp, btnp, kn), bttplp->keyp, btp->bthdr.keysize);
		}
		*bt_kychildp(btnp, kn) = bttplp->child;
	}

	return 0;
}
