/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)btdemo.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#ifdef AC_STDLIB
#include <stdlib.h>
#endif
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <blkio.h>
#include <btree.h>

#ifndef AC_STRING
/* memmove:  memory move */
#ifdef AC_PROTO
void *memmove(void *t, const void *s, size_t n)
#else
void *memmove(t, s, n)
void *t;
const void *s;
size_t n;
#endif
{
	int i = 0;

	if (t < s) {
		for (i = 0; i < n; ++i) {
			*((char *)t + i) = *((char *)s + i);
		}
	} else {
		for (i = n - 1; i >= 0; --i) {
			*((char *)t + i) = *((char *)s + i);
		}
	}

	return t;
}
#endif

/* function declarations */
#ifdef AC_PROTO
int bdlock(btree_t *btp, int ltype);
int putmenu(void);
#else
int bdlock();
int putmenu();
#endif

/* comparison function */
#ifdef AC_PROTO
int cmp(const void *p1, const void *p2, size_t n)
#else
int cmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	return strncmp((char *)p1, (char *)p2, n);
}

/* constants */
#define LCKTRIES_MAX	(100)		/* maximum lock tries */
#define PROGNAME	"btdemo"	/* default program name */
#define USAGE		"usage: %s filename keysize\n"
					/* usage message */

/* btdemo user requests */
#define REQ_NEXT	('N')	/* next */
#define REQ_PREV	('P')	/* previous */
#define REQ_FIRST	('F')	/* first */
#define REQ_LAST	('L')	/* last */
#define REQ_INS		('I')	/* insert */
#define REQ_DEL		('D')	/* delete */
#define REQ_SRCH	('S')	/* search */
#define REQ_CUR		('C')	/* current */
#define REQ_ALL		('A')	/* all */
#define REQ_MENU	('M')	/* help */
#define REQ_QUIT	('Q')	/* quit */

/*man---------------------------------------------------------------------------
NAME
     btdemo - btree demonstration program

SYNOPSIS
     btdemo filename keysize

DESCRIPTION
     btdemo is a demonstration program for the btree library.  It
     allows interactive direct access to a btree with keys having a
     single field of string data sorted by strncmp in ascending order.

SEE ALSO
     btree.

WARNING
     btdemo should be used only on btrees with keys having a single
     field of string data sorted by strncmp in ascending order.  Use
     on other btrees will produce unpredictable results and corrupt
     the file if modifications are attempted.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int main(int argc, char *argv[])
#else
int main(argc, argv)
int argc;
char *argv[];
#endif
{
	char *		progname= PROGNAME;	/* program name */
	btree_t *	btp	= NULL;		/* btree pointer */
	char		buf[256];		/* input buffer */
	char *		endp	= NULL;		/* pointer for strtol fct */
	char filename[FILENAME_MAX + 1];	/* btree file name */
	int		fldc	= 1;		/* field count */
	btfield_t	fldv[1];		/* field definition list */
	int		found	= 0;		/* key found flag */
	void *		key	= NULL;	
	long		keylen	= 0;		/* strtol return */
	size_t		keysize	= 0;		/* key size */
	const int	m	= 4;		/* btree order */
	int		rq	= 0;		/* user request */

	/* register termination function to flush blkio buffers */
#ifdef AC_STDLIB
	if (atexit(bcloseall)) {
		fputs("Unable to register termination function to flush database buffers.\n", stderr);
		exit(EXIT_FAILURE);
	}
#else
#define exit(status)	bexit(status)
#endif

	/* process command line */
	if (argc > 0) {		/* program name */
		progname = *argv;
		--argc;
		++argv;
	}
	if (argc < 1) {		/* filename */
		fprintf(stderr, USAGE, progname);
		exit(EXIT_FAILURE);
	}
	strncpy(filename, *argv, sizeof(filename));
	filename[sizeof(filename) - 1] = NUL;
	--argc;
	++argv;
	if (argc < 1) {		/* keysize */
		fprintf(stderr, USAGE, progname);
		exit(EXIT_FAILURE);
	}
	errno = 0;
	keylen = strtol(*argv, &endp, 10);
	if (errno == ERANGE) {
		fprintf(stderr, "%s: keysize argument out of range.\n", progname);
		fprintf(stderr, USAGE, progname);
		exit(EXIT_FAILURE);
	}
	if (endp != NULL) {
		if (endp[0] != NUL) {
			fprintf(stderr, USAGE, progname);
			exit(EXIT_FAILURE);
		}
	}
	if (keylen < 1) {
		fprintf(stderr, USAGE, progname);
		exit(EXIT_FAILURE);
	}
	keysize = (size_t)keylen;
	--argc;
	++argv;
	if (argc != 0) {
		fprintf(stderr, USAGE, progname);
		exit(EXIT_FAILURE);
	}

	/* open btree */
	fldc = 1;
	fldv[0].offset = 0;
	fldv[0].len = keysize;
	fldv[0].cmp = cmp;
	fldv[0].flags = BT_FASC;
	btp = btopen(filename, "r+", fldc, fldv);
	if (btp == NULL) {
		if (errno != ENOENT) {
			fprintf(stderr, "*** Error %d opening btree.\n", errno);
			exit(EXIT_FAILURE);
		}
		printf("btree %s does not exist.  Creating.\n\n", filename);
		if (btcreate(filename, m, keysize, fldc, fldv) == -1) {
			fprintf(stderr, "*** Error %d creating btree.\n", errno);
			exit(EXIT_FAILURE);
		}
		btp = btopen(filename, "r+", fldc, fldv);
		if (btp == NULL) {
			fprintf(stderr, "*** Error %d opening btree.\n", errno);
			exit(EXIT_FAILURE);
		}
	}
	/* check key size */
	if (bdlock(btp, BT_RDLCK) == -1) {	/* must lock once to read key size from file */
		fprintf(stderr, "*** Error %d read locking btree.\n", errno);
		exit(EXIT_FAILURE);
	}
	if (bdlock(btp, BT_UNLCK) == -1) {
		fprintf(stderr, "*** Error %d read locking btree.\n", errno);
		exit(EXIT_FAILURE);
	}
	if (keysize != btkeysize(btp)) {
		printf("Incorrect keysize.  Keysize for this btree is %lu.\n", (unsigned long)btkeysize(btp));
		exit(EXIT_FAILURE);
	}

	/* allocate key storage */
	key = calloc(btkeysize(btp), (size_t)1);
	if (key == NULL) {
		fprintf(stderr, "Insufficient memory for key.\n");
		exit(EXIT_FAILURE);
	}

	puts("\n--------------------------------------------------");
	puts("|           btree Library Demo Program           |");
	puts("--------------------------------------------------\n");
	putmenu();

	/* main loop */
	for (;;) {
		fputs("Enter selection:  ", stdout);
		fgets(buf, (int)sizeof(buf), stdin);
		if (buf[strlen(buf) - 1] == '\n') {
			buf[strlen(buf) - 1] = NUL;
		}
		/* add string library function to extract leading spaces */
		rq = toupper(*buf);
		if (rq == REQ_QUIT) {	/* quit btdemo */
			break;
		}
		if (rq == NUL) {	/* default to next key */
			rq = REQ_NEXT;
		}
#if 0
		if (rq == 'X') {
			if (bt_dgbtree(btp, btp->bthdr.root, stdout) == -1) {
				perror("bt_dgbtree");
				exit(EXIT_FAILURE);
			}
			continue;
		}
#endif
		switch (rq) {
		case REQ_NEXT:	/* next key */
			if (bdlock(btp, BT_RDLCK) == -1) {
				fprintf(stderr, "*** Error %d read locking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (btkeycnt(btp) == 0) {
				printf("The btree is empty.\n\n");
				if (bdlock(btp, BT_UNLCK) == -1) {
					fprintf(stderr, "*** Error %d unlocking btree.\n", errno);
					exit(EXIT_FAILURE);
				}
				break;
			}
			found = btsearch(btp, key);
			if (found == -1) {
				fprintf(stderr, "*** Error %d searching for key.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (found == 1) {
				if (btnext(btp) == -1) {
					fprintf(stderr, "*** Error %d finding next key.\n", errno);
					exit(EXIT_FAILURE);
				}
			}
			if (btcursor(btp) == NULL) {
				puts("On last key.\n");
			} else {
				if (btgetk(btp, key) == -1) {
					fprintf(stderr, "*** Error %d reading key.\n", errno);
					exit(EXIT_FAILURE);
				}
				printf("The next key is %s.\n\n", (char *)key);
			}
			if (bdlock(btp, BT_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			break;	/* case REQ_NEXT: */
		case REQ_PREV:	/* previous key */
			if (bdlock(btp, BT_RDLCK) == -1) {
				fprintf(stderr, "*** Error %d read locking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (btkeycnt(btp) == 0) {
				puts("The btree is empty.\n");
				if (bdlock(btp, BT_UNLCK) == -1) {
					fprintf(stderr, "*** Error %d unlocking btree.\n", errno);
					exit(EXIT_FAILURE);
				}
				break;
			}
			found = btsearch(btp, key);
			if (found == -1) {
				fprintf(stderr, "*** Error %d searching for key.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (found == 1) {
				if (btprev(btp) == -1) {
					fprintf(stderr, "*** Error %d finding previous key.\n", errno);
					exit(EXIT_FAILURE);
				}
			}
			if (btcursor(btp) == NULL) {
				puts("On first key.\n");
			} else {
				if (btgetk(btp, key) == -1) {
					fprintf(stderr, "*** Error %d reading key.\n", errno);
					exit(EXIT_FAILURE);
				}
				printf("The previous key is %s.\n\n", (char *)key);
			}
			if (bdlock(btp, BT_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			break;	/* case REQ_PREV: */
		case REQ_FIRST:	/* first key */
			if (bdlock(btp, BT_RDLCK) == -1) {
				fprintf(stderr, "*** Error %d read locking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (btkeycnt(btp) == 0) {
				puts("The btree is empty.\n");
				if (bdlock(btp, BT_UNLCK) == -1) {
					fprintf(stderr, "*** Error %d unlocking btree.\n", errno);
					exit(EXIT_FAILURE);
				}
				break;
			}
			if (btfirst(btp) == -1) {
				fprintf(stderr, "*** Error %d finding first key.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (btgetk(btp, key) == -1) {
				fprintf(stderr, "*** Error %d reading key.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (bdlock(btp, BT_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			printf("The first key is %s.\n\n", (char *)key);
			break;	/* case REQ_FIRST: */
		case REQ_LAST:	/* last key */
			if (bdlock(btp, BT_RDLCK) == -1) {
				fprintf(stderr, "*** Error %d read locking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (btkeycnt(btp) == 0) {
				puts("The btree is empty.\n");
				if (bdlock(btp, BT_UNLCK) == -1) {
					fprintf(stderr, "*** Error %d unlocking btree.\n", errno);
					exit(EXIT_FAILURE);
				}
				break;
			}
			if (btlast(btp) == -1) {
				fprintf(stderr, "*** Error %d finding last key.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (btgetk(btp, key) == -1) {
				fprintf(stderr, "*** Error %d reading key.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (bdlock(btp, BT_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			printf("The last key is %s.\n", (char *)key);
			break;	/* case REQ_LAST: */
		case REQ_INS:	/* insert key */
			printf("Enter key to insert:  ");
			memset(key, 0, btkeysize(btp));
			fgets(buf, (int)sizeof(buf), stdin);
			if (buf[strlen(buf) - 1] == '\n') {
				buf[strlen(buf) - 1] = NUL;
			}
			strncpy((char *)key, buf, btkeysize(btp));
			((char *)key)[btkeysize(btp) - 1] = NUL;
			if (bdlock(btp, BT_WRLCK) == -1) {
				fprintf(stderr, "*** Error %d write locking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (btinsert(btp, key) == -1) {
				if (errno == BTEDUP) {
					printf("%s is already in the btree.\n\n", (char *)key);
				} else {
					fprintf(stderr, "*** Error %d inserting key into btree.\n", errno);
					exit(EXIT_FAILURE);
				}
			} else {
				printf("%s inserted in btree.\n\n", (char *)key);
			}
			if (bdlock(btp, BT_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			break;	/* case REQ_INS: */
		case REQ_DEL:	/* delete key */
			fputs("Enter key to delete:  ", stdout);
			memset(key, 0, btkeysize(btp));
			fgets(buf, (int)sizeof(buf), stdin);
			if (buf[strlen(buf) - 1] == '\n') {
				buf[strlen(buf) - 1] = NUL;
			}
			strncpy((char *)key, buf, btkeysize(btp));
			((char *)key)[btkeysize(btp) - 1] = NUL;
			if (bdlock(btp, BT_WRLCK) == -1) {
				fprintf(stderr, "*** Error %d write locking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (btdelete(btp, key) == -1) {
				if (errno == BTENKEY) {
					printf("%s not found.\n\n", (char *)key);
				} else {
					fprintf(stderr, "*** Error %d deleting key.\n", errno);
					exit(EXIT_FAILURE);
				}
			} else {
				printf("%s deleted from btree.\n\n", (char *)key);
			}
			if (bdlock(btp, BT_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			break;	/* case REQ_DEL: */
		case REQ_SRCH:	/* search for key */
			fputs("Enter search key:  ", stdout);
			memset(key, 0, btkeysize(btp));
			fgets(buf, (int)sizeof(buf), stdin);
			if (buf[strlen(buf) - 1] == '\n') {
				buf[strlen(buf) - 1] = NUL;
			}
			strncpy((char *)key, buf, btkeysize(btp));
			((char *)key)[btkeysize(btp) - 1] = NUL;
			if (bdlock(btp, BT_RDLCK) == -1) {
				fprintf(stderr, "*** Error %d read locking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			found = btsearch(btp, key);
			if (found == -1) {
				fprintf(stderr, "*** Error %d searching for key.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (found == 1) {
				printf("%s found.\n", (char *)key);
			} else {
				printf("%s not found.\n", (char *)key);
			}
			if (bdlock(btp, BT_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			break;	/* case REQ_SRCH: */
		case REQ_CUR:	/* current key */
			if (bdlock(btp, BT_RDLCK) == -1) {
				fprintf(stderr, "*** Error %d read locking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			found = btsearch(btp, key);
			if (found == -1) {
				fprintf(stderr, "*** Error %d searching for key.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (found == 1) {
				printf("Current key: %s.\n", (char *)key);
			} else {
				printf("No current key.\n");
			}
			if (bdlock(btp, BT_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			break;	/* case REQ_CUR: */
		case REQ_ALL:	/* list all keys */
			if (bdlock(btp, BT_RDLCK) == -1) {
				fprintf(stderr, "*** Error %d read locking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			printf("There are %lu keys in the btree.\n\n", btkeycnt(btp));
			if (btsetcur(btp, NULL) == -1) {
				fprintf(stderr, "*** Error %d setting cursor.\n", errno);
				exit(EXIT_FAILURE);
			}
			for (;;) {
				if (btnext(btp) == -1) {
					fprintf(stderr, "*** Error %d finding next key.\n", errno);
					exit(EXIT_FAILURE);
				}
				if (btcursor(btp) == NULL) {
					break;
				}
				if (btgetk(btp, key) == -1) {
					fprintf(stderr, "*** Error %d reading key.\n", errno);
					exit(EXIT_FAILURE);
				}
				puts((char *)key);
			}
			putchar((int)'\n');
			if (bdlock(btp, BT_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking btree.\n", errno);
				exit(EXIT_FAILURE);
			}
			break;	/* case REQ_ALL: */
		case REQ_MENU:	/* menu */
			putmenu();
			break;	/* case REQ_MENU: */
		default:
			printf("Invalid selection.\a\n");
			break;	/* default: */
		}
	}

	/* close btree */
	if (btclose(btp) == -1) {
		fprintf(stderr, "*** Error %d closing btree.\n", errno);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

/* bdlock:  btdemo lock */
#ifdef AC_PROTO
int bdlock(btree_t *btp, int ltype)
#else
int bdlock(btp, ltype)
btree_t *btp;
int ltype;
#endif
{
	int i = 0;

	for (i = 0; i < LCKTRIES_MAX; ++i) {
		if (btlock(btp, ltype) == -1) {
			if (errno == EAGAIN) {
				continue;
			}
			return -1;
		} else {
			errno = 0;
			return 0;
		}
	}

	errno = EAGAIN;
	return -1;
}

/* putmenu:  display menu */
#ifdef AC_PROTO
int putmenu(void)
#else
int putmenu()
#endif
{
	puts("-----------------------MENU-----------------------");
	puts("| N - Next key           P - Previous key        |");
	puts("| F - First key          L - Last key            |");
	puts("| I - Insert key         D - Delete key          |");
	puts("| S - Search for key     C - Current key         |");
	puts("| A - display All keys                           |");
	puts("| M - Menu               Q - Quit                |");
	puts("--------------------------------------------------");

	return 0;
}
