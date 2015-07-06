/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)dgops.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <ctype.h>
#include <stdio.h>
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <blkio.h>

/* local headers */
#include "btree_.h"

/* function declarations */
#ifdef AC_PROTO
static void printkey(FILE *fp, const char *buf, size_t n);
#else
static void printkey();
#endif

/*man---------------------------------------------------------------------------
NAME
     bt_dgbtp - btree diagnostics for control structure

SYNOPSIS
     #include "btree_.h"

     void bt_dgbtp(btp, fp)
     btree_t *btp;
     FILE *fp;

DESCRIPTION
     The bt_dgbtp function writes the contents of btree control
     structure btp in a printable format to file fp.

SEE ALSO
     bt_dgbtree, bt_dgnode, bt_dgtuple.

PORTABILITY WARNING
     bt_dgbtp assumes that size_t is unsigned int.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
void bt_dgbtp(btree_t *btp, FILE *fp)
#else
void bt_dgbtp(btp, fp)
btree_t *btp;
FILE *fp;
#endif
{
	fputs("------------------------------------\n", fp);
	fputs("| btree control structure contents |\n", fp);
	fputs("|----------------------------------|\n", fp);
	if (!bt_valid(btp)) {
		fprintf(fp, "|  btp          = %p.\n", btp);
		fputs("|  Invalid btree pointer           |\n", fp);
		fputs("------------------------------------\n", fp);
		return;
	}
	fprintf(fp, "|  hdr.flh      = %lu.\n", btp->bthdr.flh);
	fprintf(fp, "|  hdr.m        = %d.\n", btp->bthdr.m);
	fprintf(fp, "|  hdr.keysize  = %u.\n", btp->bthdr.keysize);
	fprintf(fp, "|  hdr.flags    = %Xh.\n", btp->bthdr.flags);
	fprintf(fp, "|  hdr.root     = %lu.\n", btp->bthdr.root);
	fprintf(fp, "|  hdr.first    = %lu.\n", btp->bthdr.first);
	fprintf(fp, "|  hdr.last     = %lu.\n", btp->bthdr.last);
	fprintf(fp, "|  hdr.keycnt   = %lu.\n", btp->bthdr.keycnt);
	fprintf(fp, "|  hdr.height   = %lu.\n", btp->bthdr.height);
	fprintf(fp, "|  bp           = %p.\n", btp->bp);
	fprintf(fp, "|  flags        = %Xh.\n", btp->flags);
	fprintf(fp, "|  fldc         = %d.\n", btp->fldc);
	fprintf(fp, "|  fldv         = %p.\n", btp->fldv);
	fprintf(fp, "|  cbtpos.node  = %lu.\n", btp->cbtpos.node);
	fprintf(fp, "|  cbtpos.key   = %d.\n", btp->cbtpos.key);
	fprintf(fp, "|  cbtnp        = %p.\n", btp->cbtnp);
	fprintf(fp, "|  sp           = %p.\n", btp->sp);
	fputs("------------------------------------\n", fp);

	return;
}

/*man---------------------------------------------------------------------------
NAME
     bt_dgbtree - btree diagnostics for tree

SYNOPSIS
     #include "btree_.h"

     int bt_dgbtree(btp, node, fp)
     btree_t *btp;
     bpos_t node;
     FILE *fp;

DESCRIPTION
     The bt_dgbtree command dumps the contents of btree file to the
     text file associated with FILE pointer fp.  The contents of each
     node is written, starting with node node and descending in a
     preorder traversal of the tree.

SEE ALSO
     bt_dgbtp, bt_dgnode, bt_dgtuple.

EXAMPLE
     The following call will display the contents of an entire tree on
     the screen.

          bt_dgbtree(btp, btp->bthdr.root, stdout);

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int bt_dgbtree(btree_t *btp, bpos_t node, FILE *fp)
#else
int bt_dgbtree(btp, node, fp)
btree_t *btp;
bpos_t node;
FILE *fp;
#endif
{
	btnode_t *btnp = NULL;
	int i = 0;

	/* check for bottom of tree */
	if (node == NIL) {
		return 0;
	}

	/* read node and display */
	btnp = bt_ndalloc(btp);
	if (btnp == NULL) {
		BTEPRINT;
		return -1;
	}
	if (bt_ndget(btp, node, btnp) == -1) {
		BTEPRINT;
		bt_ndfree(btnp);
		return -1;
	}
	fprintf(fp, "Node block:  %lu.\n", node);
	bt_dgnode(btp, btnp, fp);
	fputc('\n', fp);

	/* recurse rest of tree */
	for (i = 0; i < btp->bthdr.m; ++i) {
		if (bt_dgbtree(btp, *bt_kychildp(btnp, i), fp) == -1) {
			return -1;
		}
	}

	bt_ndfree(btnp);
	btnp = NULL;

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     bt_dgnode - btree diagnostics for node

SYNOPSIS
     #include "btree_.h"

     void bt_dgnode(btp, btnp, fp)
     btree_t *btp;
     btnode_t *btnp;
     FILE *fp;

DESCRIPTION
     The bt_dgnode function writes the contents of node btnp of btree
     btp in a printable format to file fp.

SEE ALSO
     bt_dgbtp, bt_dgbtree, bt_dgtuple.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
void bt_dgnode(btree_t *btp, btnode_t *btnp, FILE *fp)
#else
void bt_dgnode(btp, btnp, fp)
btree_t *btp;
btnode_t *btnp;
FILE *fp;
#endif
{
	int i = 0;

	fputs("--------------------------\n", fp);
	fputs("| btree node contents    |\n", fp);
	fputs("--------------------------\n", fp);
	if (!bt_valid(btp)) {
		fprintf(fp, "|  btp          = %p.\n", btp);
		fputs("|  Invalid btree pointer |\n", fp);
		fputs("--------------------------\n", fp);
		return;
	}
	if (btnp == NULL) {
		fputs("|  btnp         = NULL   |\n", fp);
		fputs("--------------------------\n", fp);
		return;
	}
	if (btnp->lsib == NIL) {
		fprintf(fp, "|  lsib         = NIL\n");
	} else {
		fprintf(fp, "|  lsib         = %lu\n", btnp->lsib);
	}
	if (btnp->rsib == NIL) {
		fprintf(fp, "|  rsib         = NIL\n");
	} else {
		fprintf(fp, "|  rsib         = %lu\n", btnp->rsib);
	}
	fprintf(fp, "|  n            = %d\n", btnp->n);
	fprintf(fp, "|  keyv         = %p\n", btnp->keyv);
	fprintf(fp, "|  childv       = %p\n", btnp->childv);
	if (btnp->keyv == NULL || btnp->childv == NULL) {
		fputs("--------------------------\n", fp);
		return;
	}
	if (*bt_kychildp(btnp, 0) == NIL) {
		fputs("|  child[0] = NIL\n", fp);
	} else {
		fprintf(fp, "|  child[0] = %lu\n", *bt_kychildp(btnp, 0));
	}
	for (i = 1; (i <= btp->bthdr.m); ++i) {
		fprintf(fp, "|  key[%d] = \"", i);
		printkey(fp, bt_kykeyp(btp, btnp, i), btkeysize(btp));
		fputs("\"\n", fp);
		if (*bt_kychildp(btnp, i) == NIL) {
			fprintf(fp, "|  child[%d] = NIL\n", i);
		} else {
			fprintf(fp, "|  child[%d] = %lu\n", i, *bt_kychildp(btnp, i));
		}
	}
	fputs("--------------------------\n", fp);

	return;
}

/*man---------------------------------------------------------------------------
NAME
     bt_dgtuple - btree diagnostics for tuple

SYNOPSIS
     #include "btree_.h"

     void bt_dgtuple(btp, bttplp, fp)
     btree_t *btp;
     bttpl_t *bttplp;
     FILE *fp;

DESCRIPTION
     The bt_dgtuple function writes the contents of tuple bttplp of
     btree btp in a printable format to file fp.

SEE ALSO
     bt_dgbtp, bt_dgbtree, bt_dgnode.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
void bt_dgtuple(btree_t *btp, bttpl_t *bttplp, FILE *fp)
#else
void bt_dgtuple(btp, bttplp, fp)
btree_t *btp;
bttpl_t *bttplp;
FILE *fp;
#endif
{
	fputs("--------------------------\n", fp);
	fputs("|btree bttpl contents   |\n", fp);
	fputs("|------------------------|\n", fp);
	if (!bt_valid(btp)) {
		fprintf(fp, "|  btp          = %p.\n", btp);
		fputs("|  Invalid btree pointer |\n", fp);
		fputs("--------------------------\n", fp);
		return;
	}
	if (bttplp == NULL) {
		fputs("|  bttplp       = NULL   |\n", fp);
		fputs("--------------------------\n", fp);
		return;
	}
	if (bttplp->keyp == NULL) {
		fputs("|  bttplp->keyp = NULL   |\n", fp);
		fputs("--------------------------\n", fp);
		return;
	}
	fputs("|  key          = \"", fp);
	printkey(fp, bttplp->keyp, btkeysize(btp));
	fputs("\"\n", fp);
	if (bttplp->child == NIL) {
		fputs("|  child        = NIL    |\n", fp);
	} else {
		fprintf(fp, "|  child        = %lu.\n", bttplp->child);
	}
	fputs("--------------------------\n", fp);

	return;
}

/* printkey:  print key */
#ifdef AC_PROTO
static void printkey(FILE *fp, const char *buf, size_t n)
#else
static void printkey(fp, buf, n)
FILE *fp;
const char *buf;
size_t n;
#endif
{
	int i = 0;
	int c = 0;

	for (i = 0; i < n; ++i) {
		c = buf[i];
		switch (c) {
#ifdef AC_ESCAPE
		case '\a':	/* BEL */
			fprintf(fp, "\\a");
			break;
#endif
		case '\b':	/* BS */
			fprintf(fp, "\\b");
			break;
		case '\f':	/* FF */
			fprintf(fp, "\\f");
			break;
		case '\n':	/* NL */
			fprintf(fp, "\\n");
			break;
		case '\r':	/* CR */
			fprintf(fp, "\\r");
			break;
		case '\t':	/* HT */
			fprintf(fp, "\\t");
			break;
#ifdef AC_ESCAPE
		case '\v':	/* VT */
			fprintf(fp, "\\v");
			break;
#endif
		case '\\':
			fprintf(fp, "\\\\");
			break;
		case '\"':
			fprintf(fp, "\\\"");
			break;
		default:
			if (isprint(c)) {
				fputc(c, fp);
			} else {
				fprintf(fp, "\\%.3o", c);
			}
			break;
		}
	}

	return;
}
