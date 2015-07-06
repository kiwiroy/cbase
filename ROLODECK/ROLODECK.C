/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)rolodeck.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#ifdef AC_STDLIB
#include <stdlib.h>
#endif
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <blkio.h>
#include <cbase.h>

/* local headers */
#include "basstr.h"
#include "rolodeck.h"
#include "rolodeck.i"

/* constants */
#define EXPFILE		"rolodeck.txt"	/* export file */
#define LCKTRIES_MAX	(50)		/* maximum lock tries */
#define PROGNAME	"rolodeck"	/* default program name */
#define USAGE		"usage: %s\n"	/* usage message */
#define ADDRLIN_MAX	(2)		/* lines in address field */
#define ADDRCOL_MAX	(40)		/* columns in address field */
#define NOTSLIN_MAX	(4)		/* lines in notes field */
#define NOTSCOL_MAX	(40)		/* columns in notes field */

/* rolodeck user requests */
#define REQ_NEXT_CARD	('N')		/* next card */
#define REQ_PREV_CARD	('P')		/* previous card */
#define REQ_FIRST_CARD	('F')		/* first card*/
#define REQ_LAST_CARD	('L')		/* last card */
#define REQ_INS_CARD	('I')		/* insert card */
#define REQ_DEL_CARD	('D')		/* delete card */
#define REQ_SRCH_CARD	('S')		/* search for card */
#define REQ_TOG_SORT	('T')		/* toggle sort field */
#define REQ_CUR_CARD	('C')		/* current card */
#define REQ_ALL_CARDS	('A')		/* all cards */
#define REQ_IMPORT	('M')		/* import cards */
#define REQ_EXPORT	('X')		/* export cards */
#define REQ_HELP	('H')		/* help */
#define REQ_QUIT	('Q')		/* quit */

/* function declarations */
#ifdef AC_PROTO
int	fmltolfm(char *t, const char *s, size_t n);
int	lfmtofml(char *t, const char *s, size_t n);
void	getcard(struct rolodeck *rdp);
void	putcard(const struct rolodeck *rdp);
void	putmenu(int sf);
int	rdlock(cbase_t *cbp, int ltype);
#else
int	fmltolfm();
int	lfmtofml();
void	getcard();
void	putcard();
void	putmenu();
int	rdlock();
#endif

/*man---------------------------------------------------------------------------
NAME
    rolodeck - card file

SYNOPSIS
     rolodeck

DESCRIPTION
     rolodeck is an example program for the cbase library.  In order
     to allow it to be compiled without requiring a specific screen
     management library, only a minimal user interface has been
     implemented.

NOTES
     Below are listed a few of the more important points to note when
     examining the rolodeck source code.

          o White space is significant in string data.  For
            instance, " data" != "data".  Leading and
            trailing white space is therefore usually
            removed before storing a string in a database.
            Also, embedded white space may be reduced to a
            single space.  The cvtss function included with
            cbase will perform these string operations.
          o Names are often input and displayed
            first-name-first.  For them to sort correctly in
            a database, however, they must be stored
            last-name-first.  The functions fmltolfm and
            lfmtofml are included with cbase to convert
            between these two formats.
          o To prevent loss of buffered data, blkio buffers are
            flushed on exit either by registering the function
            bcloseall with atexit, or by using bexit in place of
            exit.

     The following notes concern locking.

          o Single-tasking applications can simply lock
            a cbase and leave it locked.
          o Locks are held for shortest time possible; a
            lock is never held during user input.
          o A write lock should not be used when only a read
            lock is required.
          o When a database file is unlocked, it may be
            modified by another process.  A record at a
            given file position may be deleted, and the
            empty slot possibly reused for a new record.
            Because of this, each time a file is locked,
            the current record must be located by performing
            a search on a unique key.
          o If locking multiple cbases, deadlock must be
            avoided (see The cbase Programmer's Guide).

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int main(int argc, char *argv[])
#else
int main(argc, argv)
int argc;
char *argv[];
#endif
{
	char		buf[256];		/* gp input buffer */
	cbase_t *	cbp	= NULL;		/* cbase pointer */
	int		found	= 0;		/* search flag */
	char *		progname= PROGNAME;	/* program name */
	struct rolodeck	rd;			/* rolodeck record */
	int		rq	= 0;		/* user request */
	int		sf	= 0;		/* sort field */

	/* register termination function to flush database buffers */
#ifdef AC_STDLIB
	if (atexit(bcloseall)) {
	 	fputs("Unable to register termination function to flush database file buffers.\n", stderr);
		exit(EXIT_FAILURE);
	}
#else
#define exit(status)	bexit(status)
#endif

	/* process command line options and arguments */
	if (argc > 0) {		/* program name */
		progname = *argv;
		--argc;
		++argv;
	}
	if (argc != 0) {
		fprintf(stderr, USAGE, progname);
		exit(EXIT_FAILURE);
	}

	/* open rolodeck cbase */
	cbp = cbopen(ROLODECK, "r+", RDFLDC, rdfldv);
	if (cbp == NULL) {
		if (errno != ENOENT) {
			fprintf(stderr, "*** Error %d opening rolodeck.\n", errno);
			exit(EXIT_FAILURE);
		}
		/* create rolodeck cbase */
		puts("Rolodeck does not exist.  Creating...");
		if (cbcreate(ROLODECK, sizeof(struct rolodeck), RDFLDC, rdfldv) == -1) {
			fprintf(stderr, "*** Error %d creating rolodeck.\n", errno);
			exit(EXIT_FAILURE);
		}
		cbp = cbopen(ROLODECK, "r+", RDFLDC, rdfldv);
		if (cbp == NULL) {
			fprintf(stderr, "*** Error %d opening rolodeck.\n", errno);
			exit(EXIT_FAILURE);
		}
	}

	puts("\n--------------------------------------------------");
	puts("|                    Rolodeck                    |");
	puts("--------------------------------------------------\n");

	/* set sort field */
	sf = RD_CONTACT;

	/* display menu */
	putmenu(sf);

	/* main loop */
	memset(&rd, 0, sizeof(rd));
	for (;;) {
		fputs("Enter selection:  ", stdout);
		fgets(buf, (int)sizeof(buf), stdin);
		cvtss(buf, buf, CVT_XSP | CVT_XCTL, sizeof(buf));
		rq = toupper(*buf);
		if (rq == REQ_QUIT) {	/* quit rolodeck */
			break;
		}
		if (rq == NUL) {		/* default to next card */
			rq = REQ_NEXT_CARD;
		}
		switch (rq) {
		case REQ_NEXT_CARD:	/* next card */
			if (rdlock(cbp, CB_RDLCK) == -1) {
				fprintf(stderr, "*** Error %d read locking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (cbreccnt(cbp) == 0) {
				puts("The rolodeck is empty.\n");
				if (rdlock(cbp, CB_UNLCK) == -1) {
					fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
					exit(EXIT_FAILURE);
				}
				continue;
			}
			/* use unique key field to set record cursor */
			found = cbkeysrch(cbp, RD_CONTACT, rd.rd_contact);
			if (found == -1) {
				fprintf(stderr, "*** Error %d searching for key.\n", errno);
				exit(EXIT_FAILURE);
			}
			/* align cursor of sort key */
			if (cbkeyalign(cbp, sf) == -1) {
				fprintf(stderr, "*** Error %d aligning key.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (found == 1) {
				/* advance key (and rec) cursor 1 position */
				if (cbkeynext(cbp, sf) == -1) {
					fprintf(stderr, "*** Error %d finding next card.\n", errno);
					exit(EXIT_FAILURE);
				}
			}
			if (cbrcursor(cbp) == NULL) {
				puts("End of deck.\n");
				if (rdlock(cbp, CB_UNLCK) == -1) {
					fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
					exit(EXIT_FAILURE);
				}
				continue;
			}
			if (cbgetr(cbp, &rd) == -1) {
				fprintf(stderr, "*** Error %d reading card.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (rdlock(cbp, CB_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			putcard(&rd);
			break;	/* case REQ_NEXT_CARD: */
		case REQ_PREV_CARD:	/* previous card */
			if (rdlock(cbp, CB_RDLCK) == -1) {
				fprintf(stderr, "*** Error %d read locking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (cbreccnt(cbp) == 0) {
				puts("The rolodeck is empty.\n");
				if (rdlock(cbp, CB_UNLCK) == -1) {
					fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
					exit(EXIT_FAILURE);
				}
				continue;
			}
			/* use unique key field to set record cursor */
			found = cbkeysrch(cbp, RD_CONTACT, rd.rd_contact);
			if (found == -1) {
				fprintf(stderr, "*** Error %d searching for key.\n", errno);
				exit(EXIT_FAILURE);
			}
			/* align cursor of sort key */
			if (cbkeyalign(cbp, sf) == -1) {
				fprintf(stderr, "*** Error %d aligning key.\n", errno);
				exit(EXIT_FAILURE);
			}
			/* retreat key (and rec) cursor 1 position */
			if (cbkeyprev(cbp, sf) == -1) {
				fprintf(stderr, "*** Error %d finding previous card.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (cbrcursor(cbp) == NULL) {
				puts("Beginning of deck.\n");
				if (rdlock(cbp, CB_UNLCK) == -1) {
					fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
					exit(EXIT_FAILURE);
				}
				continue;
			}
			if (cbgetr(cbp, &rd) == -1) {
				fprintf(stderr, "*** Error %d reading card.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (rdlock(cbp, CB_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			putcard(&rd);
			break;	/* case REQ_PREV_CARD: */
		case REQ_FIRST_CARD:	/* first card */
			if (rdlock(cbp, CB_RDLCK) == -1) {
				fprintf(stderr, "*** Error %d read locking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (cbreccnt(cbp) == 0) {
				puts("The rolodeck is empty.\n");
				if(rdlock(cbp, CB_UNLCK) == -1) {
					fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
					exit(EXIT_FAILURE);
				}
				continue;
			}
			if (cbkeyfirst(cbp, sf) == -1) {
				fprintf(stderr, "*** Error %d finding first card.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (cbgetr(cbp, &rd) == -1) {
				fprintf(stderr, "*** Error %d reading card.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (rdlock(cbp, CB_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			putcard(&rd);
			break;	/* case REQ_FIRST_CARD: */
		case REQ_LAST_CARD:	/* last card */
			if (rdlock(cbp, CB_RDLCK) == -1) {
				fprintf(stderr, "*** Error %d read locking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (cbreccnt(cbp) == 0) {
				puts("The rolodeck is empty.\n");
				if (rdlock(cbp, CB_UNLCK) == -1) {
					fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
					exit(EXIT_FAILURE);
				}
				continue;
			}
			if (cbkeylast(cbp, sf) == -1) {
				fprintf(stderr, "*** Error %d finding last card.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (cbgetr(cbp, &rd) == -1) {
				fprintf(stderr, "*** Error %d reading card.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (rdlock(cbp, CB_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			putcard(&rd);
			break;	/* case REQ_LAST_CARD: */
		case REQ_INS_CARD:	/* insert new card */
			getcard(&rd);
			if (strlen(rd.rd_contact) == 0) {
				puts("Contact name cannot be blank.  Card not inserted.\n");
				memset(&rd, 0, sizeof(rd));
				continue;
			}
			if (rdlock(cbp, CB_WRLCK) == -1) {
				fprintf(stderr, "*** Error %d write locking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (cbinsert(cbp, &rd) == -1) {
				if (errno == CBEDUP) {
					lfmtofml(buf, rd.rd_contact, sizeof(rd.rd_contact));
					printf("%.*s is already in the rolodeck.\n\n", sizeof(rd.rd_contact), buf);
					if (rdlock(cbp, CB_UNLCK) == -1) {
						fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
						exit(EXIT_FAILURE);
					}
					continue;
				}
				fprintf(stderr, "*** Error %d inserting card.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (rdlock(cbp, CB_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			putcard(&rd);
			break;	/* case REQ_INS_CARD: */
		case REQ_DEL_CARD:	/* delete current card */
			if (rdlock(cbp, CB_WRLCK) == -1) {
				fprintf(stderr, "*** Error %d write locking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			/* use unique key field to set record cursor */
			found = cbkeysrch(cbp, RD_CONTACT, rd.rd_contact);
			if (found == -1) {
				fprintf(stderr, "*** Error %d searching for key.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (found == 0) {
				puts("There is no current card.\n");
				if (rdlock(cbp, CB_UNLCK) == -1) {
					fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
					exit(EXIT_FAILURE);
				}
				continue;
			}
			/* delete record */
			if (cbdelcur(cbp) == -1) {
				fprintf(stderr, "*** Error %d deleting current card.\n", errno);
				exit(EXIT_FAILURE);
			}
			lfmtofml(buf, rd.rd_contact, sizeof(rd.rd_contact));
			printf("%.*s deleted from rolodeck.\n\n", sizeof(rd.rd_contact), buf);
			/* new current record */
			switch (sf) {
			default:
				sf = RD_CONTACT;
			case RD_CONTACT:
				found = cbkeysrch(cbp, RD_CONTACT, rd.rd_contact);
				if (found == -1) {
					fprintf(stderr, "*** Error %d searching for key.\n", errno);
					exit(EXIT_FAILURE);
				}
				break;
			case RD_COMPANY:
				found = cbkeysrch(cbp, RD_COMPANY, rd.rd_company);
				if (found == -1) {
					fprintf(stderr, "*** Error %d searching for key.\n", errno);
					exit(EXIT_FAILURE);
				}
				break;
			}
			/* load rd with new current card */
			if (cbrcursor(cbp) == NULL) {
				memset(&rd, 0, sizeof(rd));
			} else {
				if (cbgetr(cbp, &rd) == -1) {
					fprintf(stderr, "*** Error %d reading card.\n", errno);
					exit(EXIT_FAILURE);
				}
			}
			if (rdlock(cbp, CB_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			break;	/* case REQ_DEL_CARD: */
		case REQ_SRCH_CARD:	/* search for card */
			if (sf == RD_CONTACT) {
				fputs("Enter contact name:  ", stdout);
			} else {
				fputs("Enter company name:  ", stdout);
			}
			if (fgets(buf, sizeof(buf), stdin) == NULL) {
				fprintf(stderr, "*** Error %d reading input.\n");
				exit(EXIT_FAILURE);
			}
			cvtss(buf, buf, CVT_XLEADSP | CVT_XTRAILSP | CVT_1SP | CVT_XCTL, sizeof(buf));
			if (sf == RD_CONTACT) {
				fmltolfm(buf, buf, sizeof(buf));
			}
			/* don't lock until after user input */
			if (rdlock(cbp, CB_RDLCK) == -1) {
				fprintf(stderr, "*** Error %d read locking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (cbreccnt(cbp) == 0) {
				puts("The rolodeck is empty.\n");
				if (rdlock(cbp, CB_UNLCK) == -1) {
					fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
					exit(EXIT_FAILURE);
				}
				continue;
			}
			found = cbkeysrch(cbp, sf, buf);
			if (found == -1) {
				fprintf(stderr, "*** Error %d searching for key.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (found == 0) {
				printf("%s not found.\n\n", buf);
				if (cbrcursor(cbp) == NULL) {
					if (cbkeylast(cbp, sf) == -1) {
						fprintf(stderr, "*** Error %d finding last card.\n", errno);
						exit(EXIT_FAILURE);
					}
				}
			}
			if (cbgetr(cbp, &rd) == -1) {
				fprintf(stderr, "*** Error %d reading card.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (rdlock(cbp, CB_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			putcard(&rd);
			break;	/* case REQ_SRCH_CARD: */
		case REQ_TOG_SORT:	/* toggle sort field */
			switch (sf) {
			case RD_CONTACT:
				sf = RD_COMPANY;
				break;
			case RD_COMPANY:
				sf = RD_CONTACT;
				break;
			default:
				sf = RD_CONTACT;
				break;
			}
			putmenu(sf);
			break;	/* case REQ_TOG_SORT: */
		case REQ_CUR_CARD:	/* display current card */
			if (*rd.rd_contact == NUL) {
				puts("There is no current card.\n");
				continue;
			}
			if (rdlock(cbp, CB_RDLCK) == -1) {
				fprintf(stderr, "*** Error %d read locking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			/* use unique key field to set record cursor */
			found = cbkeysrch(cbp, RD_CONTACT, rd.rd_contact);
			if (found == -1) {
				fprintf(stderr, "*** Error %d searching for key.\n", errno);
				exit(EXIT_FAILURE);
			}
			/* check if card deleted by other process */
			if (found == 0) {
				puts("There is no current card.\n");
			} else {
				if (cbgetr(cbp, &rd) == -1) {
					fprintf(stderr, "*** Error %d reading card.\n", errno);
					exit(EXIT_FAILURE);
				}
				putcard(&rd);
			}
			if (rdlock(cbp, CB_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			break;	/* case REQ_CUR_CARD: */
		case REQ_ALL_CARDS:	/* display all cards */
			if (rdlock(cbp, CB_RDLCK) == -1) {
				fprintf(stderr, "*** Error %d read locking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			if (cbreccnt(cbp) == 0) {
				puts("The rolodeck is empty.\n");
				if (rdlock(cbp, CB_UNLCK) == -1) {
					fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
					exit(EXIT_FAILURE);
				}
				continue;
			}
			if (cbkeyfirst(cbp, sf) == -1) {
				fprintf(stderr, "*** Error %d setting key cursor.\n", errno);
				exit(EXIT_FAILURE);
			}
			while (cbkcursor(cbp, sf) != NULL) {
				if (cbgetr(cbp, &rd) == -1) {
					fprintf(stderr, "*** Error %d reading card.\n", errno);
					exit(EXIT_FAILURE);
				}
				putcard(&rd);
				if (cbkeynext(cbp, sf) == -1) {
					fprintf(stderr, "*** Error %d finding next card.\n", errno);
					exit(EXIT_FAILURE);
				}
			}
			if (rdlock(cbp, CB_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			break;	/* case REQ_ALL_CARDS: */
		case REQ_IMPORT:	/* import cards */
			if (rdlock(cbp, CB_WRLCK) == -1) {
				fprintf(stderr, "*** Error %d write locking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			printf("Importing cards from %s....\n", EXPFILE);
			if (cbimport(cbp, EXPFILE) == -1) {
				if (errno == ENOENT) {
					puts("Text file not found.\n");
				} else if (errno == CBEDUP) {
					puts("WARNING:  Duplicate card(s) in text file not imported.\n");
				} else {
					fprintf(stderr, "*** Error %d importing rolodeck from %s.\n", errno, EXPFILE);
					exit(EXIT_FAILURE);
				}
			} else {
				puts("Import complete.\n");
			}
			if (rdlock(cbp, CB_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			break;	/* cbase REQ_IMPORT: */
		case REQ_EXPORT:	/* export cards */
			if (rdlock(cbp, CB_RDLCK) == -1) {
				fprintf(stderr, "*** Error %d read locking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			printf("Exporting cards to %s....\n", EXPFILE);
			if (cbexport(cbp, EXPFILE) == -1) {
				fprintf(stderr, "*** Error %d exporting rolodeck to %s.\n", errno, EXPFILE);
				exit(EXIT_FAILURE);
			}
			puts("Export complete.\n");
			if (rdlock(cbp, CB_UNLCK) == -1) {
				fprintf(stderr, "*** Error %d unlocking rolodeck.\n", errno);
				exit(EXIT_FAILURE);
			}
			break;	/* cbase REQ_EXPORT: */
		case REQ_HELP:	/* help */
			putmenu(sf);
			continue;
			break;	/* case REQ_HELP: */
		default:
			putchar('\a');	/* beep */
			continue;
			break;	/* default: */
		}
	}

	/* close cbase */
	if (cbclose(cbp) == -1) {
		fprintf(stderr, "*** Error %d closing rolodeck.\n", errno);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

/* rdlock:  lock rolodeck */
#ifdef AC_PROTO
int rdlock(cbase_t *cbp, int ltype)
#else
int rdlock(cbp, ltype)
cbase_t *cbp;
int ltype;
#endif
{
	int i = 0;

	for (i = 0; i < LCKTRIES_MAX; i++) {
		if (cblock(cbp, ltype) == -1) {
			if (errno == EAGAIN) {
				continue;
			}
			return -1;
		} else {
			return 0;
		}
	}

	errno = EAGAIN;
	return -1;
}

/* putmenu:  display menu */
#ifdef AC_PROTO
void putmenu(int sf)
#else
void putmenu(sf)
int sf;
#endif
{
	puts("-----------------------MENU-----------------------");
	puts("| N - Next card          P - Previous card       |");
	puts("| F - First card         L - Last card           |");
	puts("| I - Insert new card    D - Delete current card |");
	puts("| S - Search for card    T - Toggle sort field   |");
	puts("| C - display Current    A - display All cards   |");
	puts("| M - iMport cards       X - eXport cards        |");
	puts("| H - Help (menu)        Q - Quit                |");
	puts("--------------------------------------------------");
	fputs("current sort field:  ", stdout);
	switch (sf) {
	default:
		sf = RD_CONTACT;
	case RD_CONTACT:
		fputs("contact", stdout);
		break;
	case RD_COMPANY:
		fputs("company", stdout);
		break;
	}
	puts(".\n");

	return;
}

/* getcard:  input card */
#ifdef AC_PROTO
void getcard(struct rolodeck *rdp)
#else
void getcard(rdp)
struct rolodeck *rdp;
#endif
{
	char	buf[256];
#ifndef AC_PRINTF
	char	fmt[41];
#endif
	int	i	= 0;

	/* initialize record */
	memset(rdp, 0, sizeof(*rdp));

	/* contact */
	printf("Contact   :  ");
	fgets(buf, sizeof(buf), stdin);
	cvtss(buf, buf, CVT_XLEADSP | CVT_XTRAILSP | CVT_1SP | CVT_XCTL, sizeof(buf));
	fmltolfm(rdp->rd_contact, buf, sizeof(rdp->rd_contact));

	/* title */
	printf("Title     :  ");
	fgets(buf, sizeof(buf), stdin);
	cvtss(rdp->rd_title, buf, CVT_XLEADSP | CVT_XTRAILSP | CVT_1SP | CVT_XCTL, sizeof(rdp->rd_title));

	/* company */
	printf("Company   :  ");
	fgets(buf, sizeof(buf), stdin);
	cvtss(rdp->rd_company, buf, CVT_XLEADSP | CVT_XTRAILSP | CVT_1SP | CVT_XCTL, sizeof(rdp->rd_company));

	/* street address */
	printf("Street Address (%d lines):  \n", ADDRLIN_MAX);
#ifndef AC_PRINTF
	sprintf(fmt, "%%-%d.%ds", ADDRCOL_MAX, ADDRCOL_MAX);
#endif
	for (i = 0; i < ADDRLIN_MAX; i++) {
		fgets(buf, sizeof(buf), stdin);
		cvtss(buf, buf, CVT_XLEADSP | CVT_XTRAILSP | CVT_1SP | CVT_XCTL, sizeof(buf));
#ifdef AC_PRINTF
		sprintf(rdp->rd_addr + ADDRCOL_MAX * i, "%-*.*s",
			ADDRCOL_MAX, ADDRCOL_MAX, buf);
#else
		sprintf(rdp->rd_addr + ADDRCOL_MAX * i, fmt, buf);
#endif
	}

	/* city, state zip */
	printf("City      :  ");
	fgets(buf, sizeof(buf), stdin);
	cvtss(rdp->rd_city, buf, CVT_XLEADSP | CVT_XTRAILSP | CVT_1SP | CVT_XCTL, sizeof(rdp->rd_city));
	printf("State     :  ");
	fgets(buf, sizeof(buf), stdin);
	cvtss(rdp->rd_state, buf, CVT_XSP | CVT_XCTL, sizeof(rdp->rd_state));
	printf("Zip Code  :  ");
	fgets(buf, sizeof(buf), stdin);
	cvtss(rdp->rd_zip, buf, CVT_XSP | CVT_XCTL, sizeof(rdp->rd_zip));

	/* phone extension fax */
	printf("Phone     :  ");
	fgets(buf, sizeof(buf), stdin);
	cvtss(rdp->rd_phone, buf, CVT_XSP | CVT_XCTL, sizeof(rdp->rd_phone));
	printf("Extension :  ");
	fgets(buf, sizeof(buf), stdin);
	cvtss(rdp->rd_ext, buf, CVT_XSP | CVT_XCTL, sizeof(rdp->rd_ext));
	printf("Fax       :  ");
	fgets(buf, sizeof(buf), stdin);
	cvtss(rdp->rd_fax, buf, CVT_XSP | CVT_XCTL, sizeof(rdp->rd_fax));

	/* notes */
	printf("Notes (%d lines):  \n", NOTSLIN_MAX);
#ifndef AC_PRINTF
	sprintf(fmt, "%%-%d.%ds", NOTSCOL_MAX, NOTSCOL_MAX);
#endif
	for (i = 0; i < NOTSLIN_MAX; i++) {
		fgets(buf, sizeof(buf), stdin);
		cvtss(buf, buf, CVT_XCTL, sizeof(buf));
#ifdef AC_PRINTF
		sprintf(rdp->rd_notes + NOTSCOL_MAX * i, "%-*.*s",
			NOTSCOL_MAX, NOTSCOL_MAX,
			buf);
#else
		sprintf(rdp->rd_notes + NOTSCOL_MAX * i, fmt, buf);
#endif
	}

	return;
}

/* putcard:  display card */
#ifdef AC_PROTO
void putcard(const struct rolodeck *rdp)
#else
void putcard(rdp)
const struct rolodeck *rdp;
#endif
{
#ifndef AC_PRINTF
	char	fmt[41];
#endif
	int	i	= 0;
	char name[sizeof(rdp->rd_contact)];

	lfmtofml(name, rdp->rd_contact, sizeof(name));
	puts("--------------------CARD--------------------");
#ifdef AC_PRINTF
	printf("| %-*.*s |\n", sizeof(name), sizeof(name), name);
	printf("| %-*.*s |\n", sizeof(rdp->rd_title), sizeof(rdp->rd_title),
		rdp->rd_title);
	printf("| %-*.*s |\n", sizeof(rdp->rd_company), sizeof(rdp->rd_company),
		rdp->rd_company);
	for (i = 0; i < ADDRLIN_MAX; ++i) {
		printf("| %-*.*s |\n", ADDRCOL_MAX, ADDRCOL_MAX,
			rdp->rd_addr + ADDRCOL_MAX * i);
	}
	printf("| %-*.*s, %-*.*s %-*.*s |\n",
		sizeof(rdp->rd_city), sizeof(rdp->rd_city),
		rdp->rd_city,
		sizeof(rdp->rd_state), sizeof(rdp->rd_state), rdp->rd_state,
		sizeof(rdp->rd_zip), sizeof(rdp->rd_zip), rdp->rd_zip);
	printf("| %-*.*s x%-*.*s  fax %-*.*s     |\n",
		sizeof(rdp->rd_phone), sizeof(rdp->rd_phone), rdp->rd_phone,
		sizeof(rdp->rd_ext), sizeof(rdp->rd_ext), rdp->rd_ext,
		sizeof(rdp->rd_fax), sizeof(rdp->rd_fax), rdp->rd_fax);
	for (i = 0; i < NOTSLIN_MAX; ++i) {
		printf("| %-*.*s |\n", NOTSCOL_MAX, NOTSCOL_MAX,
			rdp->rd_notes + NOTSCOL_MAX * i);
	}
#else
	/* name */
	sprintf(fmt, "| %%-%d.%ds |\n", sizeof(name), sizeof(name));
	printf(fmt, name);
	/* title */
	sprintf(fmt, "| %%-%d.%ds |\n", sizeof(rdp->rd_title), sizeof(rdp->rd_title));
	printf(fmt, rdp->rd_title);
	sprintf(fmt, "| %%-%d.%ds |\n", sizeof(rdp->rd_company), sizeof(rdp->rd_company));
	/* company */
	printf(fmt, rdp->rd_company);
	/* address */
	sprintf(fmt, "| %%-%d.%ds |\n", ADDRCOL_MAX, ADDRCOL_MAX);
	for (i = 0; i < ADDRLIN_MAX; ++i) {
		printf(fmt, rdp->rd_addr + ADDRCOL_MAX * i);
	}
	/* city, state zip */
	sprintf(fmt, "| %%-%d.%ds, %%-%d.%ds %%-%d.%ds |\n",
		sizeof(rdp->rd_city), sizeof(rdp->rd_city),
		sizeof(rdp->rd_state), sizeof(rdp->rd_state),
		sizeof(rdp->rd_zip), sizeof(rdp->rd_zip));
	printf(fmt, rdp->rd_city, rdp->rd_state, rdp->rd_zip);
	/* phone extension fax */
	sprintf(fmt, "| %%-%d.%ds x%%-%d.%ds  fax %%-%d.%ds     |\n",
		sizeof(rdp->rd_phone), sizeof(rdp->rd_phone),
		sizeof(rdp->rd_ext), sizeof(rdp->rd_ext),
		sizeof(rdp->rd_fax), sizeof(rdp->rd_fax));
	printf(fmt, rdp->rd_phone, rdp->rd_ext, rdp->rd_fax);
	/* notes */
	sprintf(fmt, "| %%-%d.%ds |\n", NOTSCOL_MAX, NOTSCOL_MAX);
	for (i = 0; i < NOTSLIN_MAX; ++i) {
		printf(fmt, rdp->rd_notes + NOTSCOL_MAX * i);
	}
#endif
	puts("--------------------------------------------");

	return;
}
