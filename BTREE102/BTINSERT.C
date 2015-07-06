/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btinsert.c	1.5 - 91/09/23" */

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

/* library headers */
#include <blkio.h>
#include <bool.h>

/* local headers */
#include "btree_.h"

/* FREE:  free all allocated storage */
#define FREE {								\
	terrno = errno;							\
	bt_ndfree(btnp);						\
	btnp = NULL;							\
	bt_ndfree(lbtnp);						\
	lbtnp = NULL;							\
	bt_ndfree(pbtnp);						\
	pbtnp = NULL;							\
	bt_ndfree(rbtnp);						\
	rbtnp = NULL;							\
	free(bttpl.keyp);						\
	bttpl.keyp = NULL;						\
	errno = terrno;							\
}

#ifdef AC_PROTO
static int setcursor(btree_t *btp, btnode_t *lbtnp, btnode_t *rbtnp)
#else
static int setcursor(btp, lbtnp, rbtnp)
btree_t *btp;
btnode_t *lbtnp;
btnode_t *rbtnp;
#endif
{
	if (btp->sp[0].key <= lbtnp->n) {
		if (bt_ndcopy(btp, btp->cbtnp, lbtnp) == -1) {
			BTEPRINT;
			return -1;
		}
		btp->cbtpos.node = btp->sp[0].node;
		btp->cbtpos.key = btp->sp[0].key;
	} else {
		if (bt_ndcopy(btp, btp->cbtnp, rbtnp) == -1) {
			BTEPRINT;
			return -1;
		}
		btp->cbtpos.node = lbtnp->rsib;
		btp->cbtpos.key = btp->sp[0].key - lbtnp->n;
	}

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     btinsert - btree insert

SYNOPSIS
     #include <btree.h>

     int btinsert(btp, buf)
     btree_t *btp;
     const void *buf;

DESCRIPTION
     The btinsert function inserts the key pointed to by buf into
     btree btp.  The cursor is positioned to the inserted key.  If the
     insertion fails, the position of the cursor is undefined.  buf
     must point to a storage area as large as the key size for the
     specified btree.

     btinsert will fail if one or more of the following is true:

     [EINVAL]       btp is not a valid btree pointer.
     [EINVAL]       buf is the NULL pointer.
     [BTEDUP]       The key at buf is already in btree btp.
     [BTELOCK]      btp is not write locked.
     [BTENOPEN]     btp is not open.

SEE ALSO
     btdelete, btsearch.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int btinsert(btree_t *btp, const void *buf)
#else
int btinsert(btp, buf)
btree_t *btp;
const void *buf;
#endif
{
	btnode_t *	btnp	= NULL;		/* node receiving new key */
	bttpl_t		bttpl;			/* btree tuple */
	bool		err	= FALSE;	/* error flag */
	int		found	= 0;		/* key found flag */
	bool		grow	= FALSE;	/* tree grow flag */
	btnode_t *	lbtnp	= NULL;		/* left sibling node */
	bpos_t		lsib	= NIL;		/* lsib location */
	bpos_t		node	= NIL;		/* node location */
	btnode_t *	pbtnp	= NULL;		/* parent node */
	int		pkn	= 0;		/* parent key number */
	bpos_t		pnode	= 0;		/* parent node location */
	btnode_t *	rbtnp	= NULL;		/* right sibling node */
	bpos_t		rsib	= NIL;		/* rsib location */
	unsigned long	spi	= 0;		/* search path index */
	int		terrno	= 0;		/* tmp errno */
	int		total	= 0;		/* total keys in node and sib */

	/* initialize automatic aggregates */
	memset(&bttpl, 0, sizeof(bttpl));

	/* validate arguments */
	if (!bt_valid(btp) || buf == NULL) {
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

	/* locate position for key and generate search path */
	found = bt_search(btp, buf);
	if (found == -1) {
		BTEPRINT;
		return -1;
	}
	if (found == 1) {			/* check for duplicate key */
		errno = BTEDUP;
		return -1;
	}

	/* create working nodes */
	btnp = bt_ndalloc(btp);
	if (btnp == NULL) {
		BTEPRINT;
		return -1;
	}
	lbtnp = bt_ndalloc(btp);	/* left sibling */
	if (lbtnp == NULL) {
		BTEPRINT;
		FREE;
		return -1;
	}
	rbtnp = bt_ndalloc(btp);	/* right sibling */
	if (rbtnp == NULL) {
		BTEPRINT;
		FREE;
		return -1;
	}
	pbtnp = bt_ndalloc(btp);	/* parent */
	if (pbtnp == NULL) {
		BTEPRINT;
		FREE;
		return -1;
	}

	/* initialize btnp with current node */
	if (bt_ndcopy(btp, btnp, btp->cbtnp) == -1) {
		BTEPRINT;
		FREE;
		return -1;
	}

	/* initialize bttpl with key to insert */
	bttpl.keyp = calloc((size_t)1, btp->bthdr.keysize);
	if (bttpl.keyp == NULL) {
		BTEPRINT;
		FREE;
		errno = ENOMEM;
		return -1;
	}
	memcpy(bttpl.keyp, buf, btp->bthdr.keysize);
	bttpl.child = NIL;

	/* set modify bit in in-core header and write to file */
	btp->bthdr.flags |= BTHMOD;
	if (bputhf(btp->bp, sizeof(bpos_t),
				(char *)&btp->bthdr + sizeof(bpos_t),
				sizeof(bthdr_t) - sizeof(bpos_t)) == -1) {
		BTEPRINT;
		FREE;
		return -1;
	}
	if (bsync(btp->bp) == -1) {
		BTEPRINT;
		FREE;
		return -1;
	}

	if (btp->bthdr.height == 0) {
		grow = TRUE;
	}

	/* loop from leaf node to root */
	for (spi = 0; spi < btp->bthdr.height; ++spi) {
		/* insert new key into node */
		if (bt_ndinskey(btp, btnp, btp->sp[spi].key, &bttpl) == -1) {
			BTEPRINT;
			err = TRUE;
			break;
		}

		/* write to disk if node not too big */
		if (btnp->n <= bt_ndmax(btp)) {
			if (bt_ndput(btp, btp->sp[spi].node, btnp) == -1) {
				BTEPRINT;
				err = TRUE;
				break;
			}
			if (spi == 0) {		/* set cursor */
				if (bt_ndcopy(btp, btp->cbtnp, btnp) == -1) {
					BTEPRINT;
					err = TRUE;
					break;
				}
				btp->cbtpos.node = btp->sp[0].node;
				btp->cbtpos.key = btp->sp[0].key;
			}
			break;
		}

		/* check if root */
		if (spi == btp->bthdr.height - 1) {
			if (bt_ndsplit(btp, btp->sp[spi].node, btnp, rbtnp, &bttpl) == -1) {
				BTEPRINT;
				err = TRUE;
				break;
			}
			if (spi == 0) {		/* set cursor */
				if (setcursor(btp, btnp, rbtnp) == -1) {
					BTEPRINT;
					err = TRUE;
					break;
				}
			}
			grow = TRUE;
			break;
		}

		/* try to shift keys with siblings */
		/* read in parent node */
		if (bt_ndget(btp, btp->sp[spi + 1].node, pbtnp) == -1) {
			BTEPRINT;
			err = TRUE;
			break;
		}
		/* get locations of nodes */
		node = btp->sp[spi].node;
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
			if (total <= 2 * bt_ndmax(btp)) {
				if (bt_ndshift(btp, btnp, rbtnp, pbtnp, pkn + 1, pnode) == -1) {
					BTEPRINT;
					err = TRUE;
					break;
				}
				if (spi == 0) {		/* set cursor */
					if (setcursor(btp, btnp, rbtnp) == -1) {
						BTEPRINT;
						err = TRUE;
						break;
					}
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
			if (total <= 2 * bt_ndmax(btp)) {
				btp->sp[spi].key = lbtnp->n + btp->sp[spi].key;
				if (bt_ndshift(btp, lbtnp, btnp, pbtnp, pkn, pnode) == -1) {
					BTEPRINT;
					err = TRUE;
					break;
				}
				if (spi == 0) {		/* set cursor */
					if (setcursor(btp, lbtnp, btnp) == -1) {
						BTEPRINT;
						err = TRUE;
						break;
					}
				}
				break;
			}
		}

		/* split node */
		if (bt_ndsplit(btp, node, btnp, rbtnp, &bttpl) == -1) {
			BTEPRINT;
			err = TRUE;
			break;
		}
		if (spi == 0) {		/* set cursor */
			if (setcursor(btp, btnp, rbtnp) == -1) {
				BTEPRINT;
				err = TRUE;
				break;
			}
		}

		/* bttpl must be inserted in pbtnp */
		if (bt_ndcopy(btp, btnp, pbtnp) == -1) {
			BTEPRINT;
			err = TRUE;
			break;
		}
	}
	if (err) {
		BTEPRINT;
		FREE;
		return -1;
	}
	bt_ndfree(btnp);	/* free in-core nodes */
	bt_ndfree(lbtnp);
	bt_ndfree(rbtnp);
	bt_ndfree(pbtnp);

	/* check if the tree grew */
	if (grow) {
		if (bt_grow(btp, &bttpl) == -1) {
			BTEPRINT;
			free(bttpl.keyp);
			return -1;
		}
		if (btp->bthdr.height == 1) {		/* set cursor */
			if (bt_ndget(btp, btp->bthdr.root, btp->cbtnp) == -1) {
				BTEPRINT;
				free(bttpl.keyp);
				return -1;
			}
			btp->cbtpos.node = btp->bthdr.root;
			btp->cbtpos.key = 1;
		}
	}
	free(bttpl.keyp);

	/* increment key count */
	btp->bthdr.keycnt++;

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

	return 0;
}
