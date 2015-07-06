/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)manx.c	1.4 - 91/08/22" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#ifdef AC_STDLIB
#include <stdlib.h>
#endif
#ifdef AC_STRING
#include <string.h>
#endif

/* function declarations */
#ifdef AC_PROTO
static int manputs(const char *cs, FILE *fp, long *pagep, long *linep);
#else
static int manputs();
#endif

/* constants */
#define PAGELEN		(60)		/* default page length */
#define BACKSLASH	('\\')		/* char to begin escape sequence */
#define PAGINATOR	"\f"		/* string to use as page separator */
#define LINELEN_MAX	(256)		/* maximum line length */
#define PROGNAME	"manx"		/* default program name */
#define USAGE		"usage: %s [language] [page-length]\n"
					/* usage message */

typedef struct {		/* language definition structure */
	char option[11];	/* language command line option */
	char begin[21];		/* marker for beginning of manual entry */
	char end[21];		/* marker for end of manual entry */
	char comment[11];	/* character sequence that begins comment */
	int  flags;		/* flags */
} lang_t;

/* lang_t bit flags */
#define LINECMT		(1)
#define BLOCKCMT	(2)

/* language definitions */
lang_t langs[] = {
	{"a",  "--man", "--end", "--", LINECMT},	/* Ada */
	{"as", ";man",  ";end",  ";",  LINECMT},	/* ASM */
	{"c",  "/*man", "*/",    "/*", BLOCKCMT},	/* C */
	{"f",  "Cman", "Cend",   "C",  LINECMT},	/* Fortran */
	{"l",  ";man", ";end",   ";",  LINECMT},	/* Lisp */
	{"m",  "(*man", "*)",    "(*", BLOCKCMT},	/* Modula-2 */
	{"p",  "{man",  "}",     "{",  BLOCKCMT},	/* Pascal */
	{"pl", "\"man", "\"",    "\"", BLOCKCMT},	/* Prolog */
	{"st", "\"man", "\"",    "\"", BLOCKCMT},	/* Smalltalk */
};

/* index into langs array for each language */
#define ADA		(1)
#define ASM		(2)
#define C		(3)
#define FORTRAN		(4)
#define LISP		(5)
#define MODULA_2	(6)
#define PASCAL		(7)
#define PROLOG		(8)
#define SMALLTALK	(9)

#define LANG		C		/* default language */

/*man---------------------------------------------------------------------------
NAME
     manx - manual entry extracter

SYNOPSIS
     manx [language] [page-length]

DESCRIPTION
     The manx command extracts manual entries from source files.  The
     source files are read from stdin and the manual entries are
     written to stdout.  Each individual manual entry is separated
     into pages by form feeds and terminated with a form feed.

     The language option specifies the language of the source file.
     The languages supported are:

                           Ada           -a
                           assembly      -as
                           C             -c
                           Fortran       -f
                           Lisp          -l
                           Modula-2      -m
                           Pascal        -p
                           Prolog        -pl
                           Smalltalk     -st

     The default language is C.

     The page-length argument can be used to set the page length.
     Pagination may be disabled by specifying a page length of 0.
     The default page length is 60.

     The beginning of a manual entry is marked by the character
     sequence (language dependent) to start a comment immediately
     followed by the characters 'man'.  This marker must appear in the
     leftmost column allowed by the language.  For block comments, the
     end of the manual entry is marked by the end of the comment.  For
     line comments, the end of a manual entry is marked by the
     character sequence to end a comment immediately followed by the
     characters 'end'.  All lines between but not including these
     constitute the manual entry.

     The following escape sequences can be used within a manual entry:

                  audible alert       BEL       \\a
                  backspace           BS        \\b
                  form feed           FF        \\f
                  carriage return     CR        \\r
                  horizontal tab      HT        \\t
                  backslash           \\         \\\\

EXAMPLES
     On a UNIX system, the following command would extract the manual
     pages from all C files in the current UNIX directory, paginate
     them to 52 lines, and place the results in a file called manual.

          $ cat *.c | manx -c 52 > manual

     Catenating files is much for difficult in DOS.  For that system,
     the following sequence of commands is required.

          > copy file.c/a+file2.c+file3.c+file4.c tmp
          > type tmp | manx -c 52 > manual
          > del tmp

     It is strongly recommended that a DOS version of cat be obtained
     for use with manx.

NOTES
     manx is particularly convenient when used in conjunction with a
     make utility.  Below are the relevant parts of the UNIX makefile
     of an actual C library for which manx is used to extract the
     reference manual.

          LIB   = blkio
          MAN   = $(LIB).man

          MANS  = blkio.h                                \\
                  bclose.c bexit.c  bflpop.c  bflpush.c  \\
                  bflush.c bgetb.c  bgetbf.c  bgeth.c    \\
                  bgethf.c bopen.c  bputb.c   bputbf.c   \\
                  bputh.c  bputhf.c bsetbuf.c bsetvbuf.c \\
                  bsync.c  lockb.c

          man:    $(MAN)

          $(MAN): $(MANS)
                  cat $(MANS) | manx > $@

     The reference manual for this library is generated with the
     command

          make man

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int main(int argc, char *argv[])
#else
int main(argc, argv)
int argc;
char *argv[];
#endif
{
	char *	endptr		= NULL;		/* pointer for strtol fct */
	int	i		= 0;		/* loop index */
	int	lang		= LANG;		/* default language */
	long	line		= 0;		/* line number */
	long	page		= 0;		/* page number */
	long	pagelen		= PAGELEN;	/* default page length */
	char *	progname	= PROGNAME;	/* program name */
	char	s[LINELEN_MAX + 1];		/* input line */

	/* process command line options and arguments */
	if (argc > 0) {				/* program name */
		progname = *argv;
		--argc;
		++argv;
	}
	if (argc > 0 && *argv[0] == '-') {	/* language option */
		lang = 0;
		for (i = 0; i < sizeof(langs) / sizeof(*langs); ++i) {
			if (strcmp(*argv + 1, langs[i].option) == 0) {
				lang = i + 1;
				break;
			}
		}
		if (lang == 0) {
			fprintf(stderr, "%s: illegal option -- %s\n", progname, *argv + 1);
			fprintf(stderr, USAGE, progname);
			exit(EXIT_FAILURE);
		}
		--argc;
		++argv;
	}
	if (argc > 0) {				/* page length argument */
		errno = 0;
		pagelen = strtol(*argv, &endptr, 10);
		if (errno == ERANGE) {
			fprintf(stderr, "%s: page length %s too large", progname, *argv);
			fprintf(stderr, USAGE, progname);
			exit(EXIT_FAILURE);
		}
		if (endptr != NULL) {
			if (endptr[0] != '\0') {
				fprintf(stderr, USAGE, progname);
				exit(EXIT_FAILURE);
			}
		}
		if (pagelen < 0) {
			fprintf(stderr, USAGE, progname);
			exit(EXIT_FAILURE);
		}
		--argc;
		++argv;
	}
	if (argc != 0) {
		fprintf(stderr, USAGE, progname);
		exit(EXIT_FAILURE);
	}

	/* main loop */
	for (;;) {
		/* read next line of input */
		if (fgets(s, (int)sizeof(s), stdin) == NULL) {
			if (ferror(stdin) != 0) {
				fprintf(stderr, "%s: *** Error reading input.  Exiting.\n", progname);
				exit(EXIT_FAILURE);
			}
			break;
		}
		/* check for manual entry marker at beginning of line */
		if (strncmp(s, langs[lang - 1].begin, strlen(langs[lang - 1].begin)) != 0) {
			continue;
		}
		if (langs[lang - 1].flags & BLOCKCMT) {
			if (strstr(s, langs[lang - 1].end) != NULL) {
				continue;
			}
		}
		/* inner loop */
		line = 0;
		page = 1;
		for (;;) {
			++line;
			/* read next line of manual entry */
			if (fgets(s, (int)sizeof(s), stdin) == NULL) {
				if (ferror(stdin) != 0) {
					fprintf(stderr, "%s: *** Error reading standard input.  Exiting.\n", progname);
					exit(EXIT_FAILURE);
				}
				break;
			}
			/* check for end of entry marker */
			if (langs[lang - 1].flags & LINECMT) {
				if (strncmp(s, langs[lang - 1].end, strlen(langs[lang - 1].end)) == 0) {
					break;
				}
				if (strncmp(s, langs[lang - 1].comment, strlen(langs[lang - 1].comment)) != 0) {
					break;
				}
			} else {
				if (strstr(s, langs[lang - 1].end) != NULL) {
					break;
				}
			}
			if (s[strlen(s) - 1] != '\n') {
				fprintf(stderr, "%s: *** Warning.  Maximum line length of %d exceeded.  Page %ld, line %ld.\n", progname, (int)(sizeof(s) - 2), page, line);
				s[strlen(s) - 1] = '\n';
			}
			if (langs[lang - 1].flags & LINECMT) {
				if (manputs(s + strlen(langs[lang - 1].comment) , stdout, &page, &line) == -1) {
					fprintf(stderr, "%s: *** Error writing line %ld, of page %ld.  Exiting\n", progname, line, page);
					exit(EXIT_FAILURE);
				}
			} else {
				if (manputs(s, stdout, &page, &line) == -1) {
					fprintf(stderr, "%s: *** Error writing line %ld, of page %ld.  Exiting\n", progname, line, page);
					exit(EXIT_FAILURE);
				}
			}
			if (line >= pagelen && pagelen != 0) {
				if (fputs(PAGINATOR, stdout) == EOF) {
					fprintf(stderr, "%s: *** Error writing paginator to standard output.  Exiting.\n", progname);
					exit(EXIT_FAILURE);
				}
				line = 0;
				++page;
			}
		}
		/* write last paginator */
		if (line != 1 && pagelen != 0) {
			if (fputs(PAGINATOR, stdout) == EOF) {
				fprintf(stderr, "%s: *** Error writing paginator to standard output.  Exiting.\n", progname);
				exit(EXIT_FAILURE);
			}
			++page;
		}
		/* check if end of file */
		if (feof(stdin) != 0) {
			break;
		}
	}

	exit(EXIT_SUCCESS);
}

/*------------------------------------------------------------------------------
NAME
     manputs - manual entry put

SYNOPSIS
     static int manputs(cs, fp, pagep, linep)
     const char *cs;
     FILE *fp;
     long *pagep;
     long *linep;

DESCRIPTION
     The manputs function writes the null-terminated string pointed to
     by cs to the named output stream.  Any manx escape sequence found
     in the string is converted to the character it represents before
     being output.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
static int manputs(const char *cs, FILE *fp, long *pagep, long *linep)
#else
static int manputs(cs, fp, pagep, linep)
const char *cs;
FILE *fp;
long *pagep;
long *linep;
#endif
{
	char	t[LINELEN_MAX + 1];	/* target string */
	int	i	= 0;		/* index into target string */
	char	c	= '\0';

	/* convert string to output format */
	for (i = 0; *cs != '\0'; ++i) {
		if (i > sizeof(t)) {
			return -1;
		}
		c = *cs++;
		/* check for escape sequence */
		if (c == BACKSLASH && *cs != '\0') {
			c = *cs++;
			switch (c) {
#ifdef AC_ESCAPE
			case 'a':	/* audible alert */
				c = '\a';
				break;
#endif
			case 'b':	/* backspace */
				c = '\b';
				break;
			case 'f':	/* form feed */
				c = '\f';
				*linep = 0;
				++*pagep;
				break;
			case 'r':	/* carriage return */
				c = '\r';
				break;
			case 't':	/* horizontal tab */
				c = '\t';
				break;
			case '\\':	/* backslash */
				c = '\\';
				break;
			default:	/* ignore backslash */
				break;
			}
		}
		t[i] = c;
	}
	t[i] = '\0';

	/* write converted string to fp */
	if (fputs(t, fp) == EOF) {
		return -1;
	}

	return 0;
}

#ifndef AC_STRING
#ifdef AC_PROTO
static char *strstr(const char *cs, const char *ct)
#else
static char *strstr(cs, ct)
const char *cs;
const char *ct;
#endif
{
	int ctlen = 0;

	if (cs == NULL || ct == NULL) {
		return NULL;
	}

	ctlen = strlen(ct);
	while (*cs != '\0') {
		if (strncmp(cs, ct, ctlen) == 0) {
			return cs;
		}
		++cs;
	}

	return NULL;
}
#endif
