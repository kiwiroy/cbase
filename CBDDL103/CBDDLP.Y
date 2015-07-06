/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* definition section --------------------------------------------------------*/
%{
/* #ident	"@(#)cbddlp.y	1.6 - 91/09/23" */

#include <ansi.h>

/* local headers */
#include "cbddlp.h"

#ifdef DEBUG
#define DBPRINT {							\
	fprintf(stderr, "%s[%d].\n", __FILE__, __LINE__);		\
}
#else
#define DBPRINT
#endif

%}

/* declaration section -------------------------------------------------------*/
%union {
	int	keyword;
	char *	buf;
}
%token	<keyword>	COMPOUND CONTAINS DATAFILE KEY INDEXFILE RECORD UNIQUE
%token	<buf>		ELEMC IDENTIFIER STRING

/* rule section --------------------------------------------------------------*/
%%
/* data definition language */
ddl	: stmt
	| ddl stmt
	;

/* generic statement */
stmt	: /* blank line */
	| datafile
	| indexfile
	| record
	;

/* date file statement */
datafile: DATAFILE STRING CONTAINS IDENTIFIER ';' {
#ifdef DEBUG
		fprintf(stderr, "Line %d: DATAFILE \"%s\" CONTAINS %s;\n", yylineno, $2, $4);
#endif
		/* add slot to data file table */
		ddldfv = (ddldf_t *)realloc(ddldfv, (dfc + 1) * sizeof(*ddldfv));
		if (ddldfv == NULL) {
			yyerror("out of memory");
			return -1;
		}
		memset(ddldfv + dfc, 0, sizeof(*ddldfv));
		ddldfv[dfc].filename = $2;
		ddldfv[dfc].record = $4;
		++dfc;
	}
	;

/* index file statement */
indexfile: INDEXFILE STRING CONTAINS IDENTIFIER ';' {
#ifdef DEBUG
		fprintf(stderr, "Line %d: INDEXFILE \"%s\" CONTAINS %s;\n", yylineno, $2, $4);
#endif
		/* add slot to index file table */
		ddlifv = (ddlif_t *)realloc(ddlifv, (ifc + 1) * sizeof(*ddlifv));
		if (ddlifv == NULL) {
			yyerror("out of memory");
			return -1;
		}
		memset(ddlifv + ifc, 0, sizeof(*ddlifv));
		ddlifv[ifc].filename = $2;
		ddlifv[ifc].key = $4;
		++ifc;
	}
	;

/* record statement */
record	: RECORD IDENTIFIER '{' fldlst '}' ';' {
		int fld = 0;
#ifdef DEBUG
		fprintf(stderr, "Line %d: RECORD %s { fldlst };\n", yylineno, $2);
#endif
		recname = $2;
		ddlputh(hfp, recname, fldc, ddlfldv);
		ddlputi(ifp, recname, fldc, ddlfldv);
		/* free memory allocated for current record */
		free(recname);
		recname = NULL;
		for (fld = 0; fld < fldc; ++fld) {
			ddlfldv[fld].flags = 0;
			free(ddlfldv[fld].type);
			ddlfldv[fld].type = NULL;
			if (isalloc(ddlfldv[fld].ctype)) {
				free(ddlfldv[fld].ctype);
			}
			ddlfldv[fld].ctype = NULL;
			free(ddlfldv[fld].name);
			ddlfldv[fld].name = NULL;
			if (ddlfldv[fld].elemc != NULL) free(ddlfldv[fld].elemc);
			ddlfldv[fld].elemc = NULL;
		}
		fldc = 0;
	}
	;

/* field list */
fldlst	: fld
	| fldlst fld
	;

/* field */
fld	: fqlst IDENTIFIER IDENTIFIER ELEMC ';' {
#ifdef DEBUG
		fprintf(stderr, "Line %d: fqlst %s %s [%s];\n", yylineno, $2, $3, $4);
#endif
		/* add slot to field table */
		ddlfldv = (ddlfld_t *)realloc(ddlfldv, (fldc + 2) * sizeof(*ddlfldv));
		if (ddlfldv == NULL) {
			yyerror("out of memory");
			return -1;
		}
		memset(ddlfldv + fldc + 1, 0, sizeof(*ddlfldv));
		ddlfldv[fldc].type = $2;
		ddlfldv[fldc].ctype = ctype(ddlfldv[fldc].type);
		if (ddlfldv[fldc].ctype == NULL) {
			yyerror("Undefined cbase data type");
			return -1;
		}
		ddlfldv[fldc].name = $3;
		ddlfldv[fldc].elemc = $4;
		++fldc;
	}
	| fqlst IDENTIFIER IDENTIFIER ';' {
#ifdef DEBUG
		fprintf(stderr, "Line %d: fqlst %s %s;\n", yylineno, $2, $3);
#endif
		/* add slot to field table */
		ddlfldv = (ddlfld_t *)realloc(ddlfldv, (fldc + 2) * sizeof(*ddlfldv));
		if (ddlfldv == NULL) {
			yyerror("out of memory");
			return -1;
		}
		memset(ddlfldv + fldc + 1, 0, sizeof(*ddlfldv));
		ddlfldv[fldc].type = $2;
		ddlfldv[fldc].ctype = ctype(ddlfldv[fldc].type);
		if (ddlfldv[fldc].ctype == NULL) {
			yyerror("Undefined cbase data type");
			return -1;
		}
		ddlfldv[fldc].name = $3;
		ddlfldv[fldc].elemc = NULL;
		++fldc;
	}
	| fqlst IDENTIFIER ':' IDENTIFIER IDENTIFIER ELEMC ';' {
#ifdef DEBUG
		fprintf(stderr, "Line %d: fqlst %s:%s %s %s;\n", yylineno, $2, $4, $5, $6);
#endif
		/* add slot to field table */
		ddlfldv = (ddlfld_t *)realloc(ddlfldv, (fldc + 2) * sizeof(*ddlfldv));
		if (ddlfldv == NULL) {
			yyerror("out of memory");
			return -1;
		}
		memset(ddlfldv + fldc + 1, 0, sizeof(*ddlfldv));
		ddlfldv[fldc].type = $2;
		ddlfldv[fldc].ctype = $4;
		ddlfldv[fldc].name = $5;
		ddlfldv[fldc].elemc = $6;
		++fldc;
	}
	| fqlst IDENTIFIER ':' IDENTIFIER IDENTIFIER ';' {
		/* add slot to field table */
		ddlfldv = (ddlfld_t *)realloc(ddlfldv, (fldc + 2) * sizeof(*ddlfldv));
		if (ddlfldv == NULL) {
			yyerror("out of memory");
			return -1;
		}
		memset(ddlfldv + fldc + 1, 0, sizeof(*ddlfldv));
		ddlfldv[fldc].type = $2;
		ddlfldv[fldc].ctype = $4;
		ddlfldv[fldc].name = $5;
		ddlfldv[fldc].elemc = NULL;
		++fldc;
	}
	;

/* field qualifier list */
fqlst	: /* none */
	| fqlst fq
	;

/* field qualifier */
fq	: COMPOUND {
#ifdef DEBUG
		fprintf(stderr, "Line %d: COMPOUND\n", yylineno);
#endif
		yyerror("compound keys not yet implemented");
		return -1;
	}
	| KEY {
#ifdef DEBUG
		fprintf(stderr, "Line %d: KEY\n", yylineno);
#endif
		ddlfldv[fldc].flags |= CB_FKEY;
	}
	| UNIQUE {
#ifdef DEBUG
		fprintf(stderr, "Line %d: UNIQUE\n", yylineno);
#endif
		ddlfldv[fldc].flags |= CB_FUNIQ;
	}
	;
%%

/* subroutine section --------------------------------------------------------*/
/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#include <stdio.h>
#ifdef AC_STDLIB
#include <stdlib.h>
#endif
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <bool.h>
#include <cbase.h>

/* local headers */
#include "basstr.h"
#include "cbddlp.h"

/* constants */
#define PROGNAME	"cbddlp"		/* default program name */
#define USAGE		"usage: %s ddlfile\n"	/* usage message */

/* global definitions */
char *		progname	= PROGNAME;	/* program name */
FILE *		hfp		= NULL;		/* .h file stream */
FILE *		ifp		= NULL;		/* .i file stream */
char *		recname		= NULL;		/* record name */
int		dfc		= 0;		/* data file count */
ddldf_t	*	ddldfv		= NULL;		/* ddl data file table */
int		ifc		= 0;		/* index file count */
ddlif_t	*	ddlifv		= NULL;		/* ddl index file table */
int		fldc		= 0;		/* field count */
ddlfld_t *	ddlfldv		= NULL;		/* ddl field table */
const char * const typtbl[] = {			/* cbase/C data type table */
	"t_char",	"char",			/*  0 */
	"t_charv",	"char",			/*  1 */
	"t_uchar",	"unsigned char",	/*  2 */
	"t_ucharv",	"unsigned char",	/*  3 */
	"t_short",	"short",		/*  4 */
	"t_shortv",	"short",		/*  5 */
	"t_ushort",	"unsigned short",	/*  6 */
	"t_ushortv",	"unsigned short",	/*  7 */
	"t_int",	"int",			/*  8 */
	"t_intv",	"int",			/*  9 */
	"t_uint",	"unsigned int",		/* 10 */
	"t_uintv",	"unsigned int",		/* 11 */
	"t_long",	"long",			/* 12 */
	"t_longv",	"long",			/* 13 */
	"t_ulong",	"unsigned long",	/* 14 */
	"t_ulongv",	"unsigned long",	/* 15 */
	"t_float",	"float",		/* 16 */
	"t_floatv",	"float",		/* 17 */
	"t_double",	"double",		/* 18 */
	"t_doublev",	"double",		/* 19 */
	"t_ldouble",	"long double",		/* 20 */
	"t_ldoublev",	"long double",		/* 21 */
	"t_pointer",	"void *",		/* 22 */
	"t_pointerv",	"void *",		/* 23 */
	"t_string",	"char",			/* 24 */
	"t_cistring",	"char",			/* 25 */
	"t_binary",	"unsigned char",	/* 26 */
};

/*man---------------------------------------------------------------------------
NAME
     cbddlp - cbase data definition language processor

SYNOPSIS
     cbddlp ddlfile

DESCRIPTION
     The cbddlp command processes cbase data definition language (DDL)
     files.  A cbase database can be specified in a DDL file, which is
     then processed using cbddlp to generate the C files required by
     cbase to manipulate that database.  cbddlp requires that DDL
     files have the suffix ".ddl".  The C files generated are given
     the same name as the DDL file except for the suffix; a .h and a
     .i file are generated.  The .h file must be included by every
     module accessing the database, while the .i file must be included
     by exactly one module of every program that accesses the database.

     There are three types of DDL statements:  data file, index file,
     and record.  The syntax for the record statement is

          record recname {
               [[unique ]key] dbtype fldname[\\[elemc\\]];
               ...
          };

     recname is the name of the record.  dbtype is the database data
     type of the field.  fldname is the name of the field, and must be
     unique for a given database.  elemc specifies the number of
     elements for array data types; this may be any C expression valid
     for defining the size of a static array.  The key keyword
     specifies that an index is to be maintained on this field.  The
     unique keyword specifies that the keys in this index must be
     unique.  Multiple records can be defined in the same DDL file.

     User-defined data types may also be specified in a DDL file, but
     require an additional piece of information.  For the predefined
     data types, cbddlp knows what the corresponding C data type is
     (t_string : char, t_int : int, etc.).  But for user-defined data
     types, this must be explicitly specified.  The syntax for this is
     as follows.

          [[unique ]key] dbtype:ctype fldname[\[elemc\]];

     where dbtype is a user-defined database data type and ctype is
     the corresponding C data type.  ctype must consist of only one
     identifier; if the C data type consists of multiple identifiers
     (e.g., long double), either #define or typedef must be used to
     reduce it to one identifier (e.g., typedef long double ldouble).

     The syntax for the data file statement is

          data file "filename" contains recname;

     filename is the name of the file in which recname records are to
     be stored.  The data file statement must precede its associated
     record statement.

     The syntax for the index file statement is

          index file "filename" contains keyname;

     filename is the name of the file in which keyname keys are to be
     indexed.  The index file statement must precede the record
     statement containing the associated key.

     C comments may appear in DDL files wherever white space is
     allowed, with the exception of within the keyword pairs "data
     file" and "index file".  C preprocessor statements may also be
     placed in DDL files, but cannot currently span multiple lines,
     nor can comments on the same line as a preprocessor statement;
     any line beginning with the # character (excluding any leading
     white space) will be passed directly through to the generated .h
     file.

     In the headers generated by cbddlp, a macro for the record name
     is constructed by converting all lower case letters in the record
     name to capitals.  A macro is also defined for each field in the
     same fashion.  The initial characters (up to four) of the first
     field name of a record preceding the first underscore are used as
     a prefix for the field count macro and field definition list.
     This prefix must be unique for all records in a database.  The
     field count macro identifier is constructed by converting this
     prefix to capitals and appending FLDC.  The field definition list
     identifier is constructed by appending fldv to the prefix.

EXAMPLE
     Below is given an example DDL file for a rolodeck database
     consisting of a single record.

     /* constants *\/
     #define NAME_MAX     (40)   /* max name length *\/
     #define ADDR_MAX     (40)   /* max address length *\/
     #define NOTELIN_MAX   (4)   /* note lines *\/
     #define NOTECOL_MAX  (40)   /* note columns *\/

     /* file assignments *\/
     data file  "rolodeck.dat" contains rolodeck;
     index file "rdcont.ndx"   contains rd_contact;
     index file "rdcomp.ndx"   contains rd_company;

     record rolodeck {                        /* rolodeck record *\/
         unique key t_string
                      rd_contact[NAME_MAX];   /* contact name *\/
         t_string     rd_title[41];           /* contact title *\/
         key t_string rd_company[NAME_MAX];   /* company name *\/
         t_string     rd_addr[81];            /* address *\/
         t_string     rd_city[26];            /* city *\/
         t_string     rd_state[3];            /* state *\/
         t_string     rd_zip[11];             /* zip code *\/
         t_string     rd_phone[13];           /* phone number *\/
         t_string     rd_ext[5];              /* phone extension *\/
         t_string     rd_fax[13];             /* fax number *\/
         t_string     rd_notes[NOTELIN_MAX * NOTECOL_MAX];
                                              /* notes *\/
     };

     If the name of this DDL file was rolodeck.ddl, the generated C
     files would be rolodeck.h and rolodeck.i.  The cbase name macro
     would be ROLODECK (the record identifier capitalized).  The field
     count macro would be RDFLDC, and the field definition list would
     be rdfldv.  The rolodeck would be therefore be opened (for
     writing) with the C statement

          cbopen(ROLODECK, "r+", RDFLDC, rdfldv);

     The phone number from the current record would be read from the
     database with

         cbgetrf(cbp, RD_PHONE, buf);


NOTES
     For a make utility to automatically process DDL files without an
     explicit rule for each one, suffix rules defining the creation of
     the data definition header files from a DDL file can be added to
     the makefile.  For the standard UNIX make, the following
     instructions would be inserted near the beginning of the
     makefile.

     # suffix rules
     .SUFFIXES:  .ddl .h .i

     .ddl.h:
          cbddlp $<

     .ddl.i:
          cbddlp $<

     The exact procedure for other make utilities varies.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int main(int argc, char *argv[])
#else
int main(argc, argv)
int argc;
char *argv[];
#endif
{
	char *	dbname		= NULL;		/* database name */
	char *	udbname		= NULL;		/* upper case database name */
	char *	ddlfile		= NULL;		/* ddl filename */
	FILE *	ddlfp		= NULL;		/* ddl file stream */
	char *	hfile		= NULL;		/* .h filename */
	char *	ifile		= NULL;		/* .i filename */
	size_t	len		= 0;		/* string length */
	char *	p1		= NULL;		/* gp pointers */
	char *	p2		= NULL;

	/* process command line */
	if (argc > 0) {			/* program name */
		progname = *argv;
		--argc;
		++argv;
	}
	if (argc < 1) {			/* ddl filename */
		fprintf(stderr, USAGE, progname);
		exit(EXIT_FAILURE);
	}
	ddlfile = *argv;
	--argc;
	++argv;
	if (argc != 0) {
		fprintf(stderr, USAGE, progname);
		exit(EXIT_FAILURE);
	}

	/* construct output filenames */
	p1 = strrchr(ddlfile, PATHDLM);		/* remove path prefix */
	if (p1 == NULL) {
		p1 = ddlfile;
	} else {
		++p1;
	}
	p2 = strrchr(p1, '.');			/* remove dot suffix */
	if (p2 == NULL) {
		fprintf(stderr, "%s: Input file must have .ddl suffix.\n", progname);
		exit(EXIT_FAILURE);
	}
	if (strcmp(p2, ".ddl") != 0) {
		fprintf(stderr, "%s: Input file must have .ddl suffix.\n", progname);
		exit(EXIT_FAILURE);
	}
	len = p2 - p1;				/* length w/o suffix */
	hfile = (char *)malloc(len + 2 + 1);	/* .h file name */
	if (hfile == NULL) {
		perror("out of memory");
		exit(EXIT_FAILURE);
	}
	strncpy(hfile, p1, len);
	strncpy(hfile + len, ".h", 2);
	hfile[len + 2] = NUL;
	ifile = (char *)malloc(len + 2 + 1);	/* .i file name */
	if (ifile == NULL) {
		perror("out of memory");
		exit(EXIT_FAILURE);
	}
	strncpy(ifile, p1, len);
	strncpy(ifile + len, ".i", 2);
	ifile[len + 2] = NUL;
	dbname = (char *)malloc(len + 1);	/* database name */
	if (dbname == NULL) {
		perror("out of memory");
		exit(EXIT_FAILURE);
	}
	strncpy(dbname, p1, len);
	dbname[len] = NUL;
	udbname = (char *)malloc(len + 1);	/* upper case database name */
	if (udbname == NULL) {
		perror("out of memory");
		exit(EXIT_FAILURE);
	}
	cvtss(udbname, dbname, CVT_UPPER, len);
	udbname[len] = NUL;

	/* allocate first slot in each ddl table */
	ddldfv = (ddldf_t *)calloc((size_t)1, sizeof(*ddldfv));
							/* data file table */
	if (ddldfv == NULL) {
		perror("out of memory");
		exit(EXIT_FAILURE);
	}
	ddlifv = (ddlif_t *)calloc((size_t)1, sizeof(*ddlifv));
							/* index file table */
	if (ddlifv == NULL) {
		perror("out of memory");
		exit(EXIT_FAILURE);
	}
	ddlfldv = (ddlfld_t *)calloc((size_t)1, sizeof(*ddlfldv));
							/* field table */
	if (ddlfldv == NULL) {
		perror("out of memory");
		exit(EXIT_FAILURE);
	}

	/* open files */
	ddlfp = fopen(ddlfile, "r");			/* ddl file */
	if (ddlfp == NULL) {
		perror("opening ddl file");
		exit(EXIT_FAILURE);
	}
	hfp = fopen(hfile, "w");			/* .h file */
	if (hfp == NULL) {
		perror("opening .h file");
		exit(EXIT_FAILURE);
	}
	ifp = fopen(ifile, "w");			/* .i file */
	if (ifp == NULL) {
		perror("opening .i file");
		exit(EXIT_FAILURE);
	}

	/* redirect lex input to ddl stream */
	yyin = ddlfp;

	/* write multiple include #ifndefs */
	fprintf(hfp, "#ifndef H_%s\t/* prevent multiple includes */\n", udbname);
	fprintf(hfp, "#define H_%s\n\n", udbname);
	fprintf(ifp, "#ifndef I_%s\t/* prevent multiple includes */\n", udbname);
	fprintf(ifp, "#define I_%s\n\n", udbname);

	/* write #includes */
	fputs("/* libray headers */\n", hfp);		/* .h file */
	fputs("#include <cbase.h>\n\n", hfp);
	fputs("#include <ansi.h>\n", ifp);		/* .i file */
	fputs("\n/* ansi headers */\n", ifp);
	fputs("#ifdef AC_STDDEF\n", ifp);
	fputs("#include <stddef.h>\n", ifp);
	fputs("#endif\n", ifp);
	fputs("\n/* libray headers */\n", ifp);
	fputs("#include <cbase.h>\n", ifp);
	fputs("\n/* local headers */\n", ifp);
	fprintf(ifp, "#include \"%s\"\n\n", hfile);

	/* process ddl file */
	if (yyparse()) {
		fprintf(stderr, "Fatal error processing %s.\n", ddlfile);
		exit(EXIT_FAILURE);
	}
	if (ferror(hfp)) {
		fprintf(stderr, "Error writing %s: ", hfile);
		perror("");
		exit(EXIT_FAILURE);
	}
	if (ferror(ifp)) {
		fprintf(stderr, "Error writing %s: ", ifile);
		perror("");
		exit(EXIT_FAILURE);
	}

	/* write multiple include #endifs */
	fprintf(hfp, "#endif	/* #ifndef H_%s */\n", udbname);
	fprintf(ifp, "#endif	/* #ifndef I_%s */\n", udbname);

	/* close files */
	if (fclose(ddlfp) == EOF) {
		fprintf(stderr, "Error closing %s: ", ddlfile);
		perror("");
		exit(EXIT_FAILURE);
	}
	if (fclose(hfp) == EOF) {
		fprintf(stderr, "Error closing %s: ", hfile);
		perror("");
		exit(EXIT_FAILURE);
	}
	if (fclose(ifp) == EOF) {
		fprintf(stderr, "Error closing %s: ", ifile);
		perror("");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

/* ctype:  get C data type from cbase data type */
#ifdef AC_PROTO
char *ctype(const char *type)
#else
char *ctype(type)
const char *type;
#endif
{
	int i = 0;

	for (i = 0; i < nelems(typtbl); i += 2) {
		if (strcmp(typtbl[i], type) == 0) {
			return typtbl[i + 1];
		}
	}

	return NULL;
}

/* fpctype:  put C type */
#ifdef AC_PROTO
int fpctype(FILE *fp, const char *type)
#else
int fpctype(fp, type)
FILE *fp;
const char *type;
#endif
{
	int i = 0;

	for (i = 0; i < nelems(typtbl); i += 2) {
		if (strcmp(typtbl[i], type) == 0) {
			fputs(typtbl[i + 1], fp);
			return 0;
		}
	}

	return -1;
}

/* fpdatafile:  file print data file name */
#ifdef AC_PROTO
int fpdatafile(FILE *fp, const char *recname)
#else
int fpdatafile(fp, recname)
FILE *fp;
const char *recname;
#endif
{
	int i = 0;

	for (i = 0; i < dfc; ++i) {
		if (strcmp(ddldfv[i].record, recname) == 0) {
			fprintf(fp, ddldfv[i].filename);
			return 0;
		}
	}

	return -1;
}

/* fpndxfile:  file print index file name */
#ifdef AC_PROTO
int fpndxfile(FILE *fp, const char *keyname)
#else
int fpndxfile(fp, keyname)
FILE *fp;
const char *keyname;
#endif
{
	int i = 0;

	for (i = 0; i < ifc; ++i) {
		if (strcmp(ddlifv[i].key, keyname) == 0) {
			fprintf(fp, ddlifv[i].filename);
			return 0;
		}
	}

	return -1;
}

/* isalloc:  is ctype allocated? */
#ifdef AC_PROTO
bool isalloc(const char *ctype)
#else
bool isalloc(ctype)
const char *ctype;
#endif
{
	int i = 0;

	for (i = 1; i < nelems(typtbl); i += 2) {
		if (typtbl[i] == ctype) {
			return FALSE;
		}
	}

	return TRUE;
}

/* tmpupr:  temporary upper case string conversion */
#ifdef AC_PROTO
char *tmpupr(const char *s)
#else
char *tmpupr(s)
const char *s;
#endif
{
	static char uprstr[81];

	cvtss(uprstr, s, CVT_UPPER, sizeof(uprstr));
	uprstr[sizeof(uprstr) - 1] = NUL;

	return uprstr;
}

/* warning:  print warning message */
#ifdef AC_PROTO
void warning(const char *s, const char *t)
#else
void warning(s, t)
const char *s;
const char *t;
#endif
{
	fprintf(stderr, "%s: %s", progname, s);
	if (t != NULL) {
		fprintf(stderr, " %s", t);
	}
	fprintf(stderr, " near line %d.\n", yylineno);

	return;
}

/* yyerror:  yacc error message */
#ifdef AC_PROTO
int yyerror(const char *s)
#else
int yyerror(s)
const char *s;
#endif
{
	warning(s, NULL);

	return 0;
}

/* global data for ddlput functions */
static char	prefix[PREFIX_MAX + 1];		/* identifier prefix */
static char	uprefix[PREFIX_MAX + 1];	/* prefix in upper case */

/* ddlputh:  write to .h file */
#ifdef AC_PROTO
int ddlputh(FILE *fp, const char *recname, int fldc, const ddlfld_t fldv[])
#else
int ddlputh(fp, recname, fldc, fldv)
FILE *fp;
const char *recname;
int fldc;
const ddlfld_t fldv[];
#endif
{
	int	fld	= 0;		/* field number */
	char *	p	= NULL;		/* gp char pointer */

#ifdef DEBUG
	/* validate arguments */
	if (fp == NULL || recname == NULL || fldc < 1 || fldv == NULL) {
		errno = EINVAL;
		return -1;
	}
#endif
	/* record name */
	fputs("/* record name */\n", fp);
	fprintf(fp, "#define %s\t\"", tmpupr(recname));
	if (fpdatafile(fp, recname) == -1) {
		fprintf(stderr, "No data file name specified for record %s.\n", recname);
		return -1;
	}
	fputs("\"\n\n", fp);

	/* extract identifier prefix from first field name */
	strncpy(prefix, fldv[0].name, sizeof(prefix));
	prefix[sizeof(prefix) - 1] = NUL;
	p = strchr(prefix, '_');
	if (p != NULL) {
		*p = NUL;
	}
	cvtss(uprefix, prefix, CVT_UPPER, sizeof(uprefix));
	uprefix[sizeof(uprefix) - 1] = NUL;

	/* record definition */
	fprintf(fp, "/* %s record definition */\n", recname);
	fprintf(fp, "typedef struct %s {\n", recname);
	for (fld = 0; fld < fldc; ++fld) {
		fputc('\t', fp);
		fputs(fldv[fld].ctype, fp);
		fprintf(fp, " %s", fldv[fld].name);
		if (fldv[fld].elemc != NULL) {
			fprintf(fp, "[%s]", fldv[fld].elemc);
		}
		fputs(";\n", fp);
	}
	fprintf(fp, "} %s_t;\n\n", recname);

	/* field names */
	fprintf(fp, "/* field names for record %s */\n", recname);
	for (fld = 0; fld < fldc; ++fld) {
		fprintf(fp, "#define %s", tmpupr(fldv[fld].name));
		if (strlen(fldv[fld].name) < 8) {	/* for tabs set at 8 */
			fputc('\t', fp);
		}
		fprintf(fp, "\t(%d)\n", fld);
	}
	fprintf(fp, "#define %sFLDC\t\t(%d)\n\n", uprefix, fldc);

	/* field definition list declaration */
	fprintf(fp, "/* field definition list for record %s */\n", recname);
	fprintf(fp, "extern cbfield_t %sfldv[%sFLDC];\n\n", prefix, uprefix);

	return 0;
}

/* ddlputi:  write to .i file */
#ifdef AC_PROTO
int ddlputi(FILE *fp, const char *recname, int fldc, const ddlfld_t fldv[])
#else
int ddlputi(fp, recname, fldc, fldv)
FILE *fp;
const char *recname;
int fldc;
const ddlfld_t fldv[];
#endif
{
	int	fld	= 0;		/* field number */

	/* field definition list definition */
	fprintf(fp, "/* field definition list for record %s */\n", recname);
	fprintf(fp, "cbfield_t %sfldv[] = {\n", prefix);
	for (fld = 0; fld < fldc; ++fld) {
		fputs("\t{\n", fp);
		fprintf(fp, "\t\toffsetof(struct %s, %s),\n", recname, fldv[fld].name);
		fprintf(fp, "\t\tsizeofm(struct %s, %s),\n", recname, fldv[fld].name);
		fprintf(fp, "\t\t%s,\n", fldv[fld].type);
		if (fldv[fld].flags & CB_FKEY) {
			fprintf(fp, "\t\tCB_FKEY");
			if (fldv[fld].flags & CB_FUNIQ) {
				fprintf(fp, " | CB_FUNIQ");
			}
			fputs(",\n\t\t\"", fp);
			if (fpndxfile(fp, fldv[fld].name) == -1) {
				fprintf(stderr, "No index file name specified for key %s.\n", fldv[fld].name);
			}
			fputs("\"\n", fp);
		} else {
			fputs("\t\t0,\n\t\tNULL\n", fp);
		}
		fprintf(fp, "\t},\n");
	}
	fprintf(fp, "};\n\n");

	return 0;
}
