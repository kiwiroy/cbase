/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

#ifndef H_CBDDLP	/* prevent multiple includes */
#define H_CBDDLP

/* #ident	"@(#)cbddlp.h	1.6 - 91/09/23" */

#include <ansi.h>

/*#define DEBUG		/* switch to enable debugging */

/* ansi headers */
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#include <stdio.h>

/* library headers */
#include <bool.h>

/* constants */
#define PATHDLM		('/')		/* file path name delimiter */
#define PREFIX_MAX	(4)		/* maximum prefix length */

/* structure definitions */
typedef struct {			/* ddl data file */
	char *	filename;		/* file name */
	char *	record;			/* record contained */
} ddldf_t;

typedef struct {			/* ddl index file */
	char *	filename;		/* file name */
	char *	key;			/* key contained */
} ddlif_t;

typedef struct {			/* ddl field */
	int	flags;			/* flags */
	char *	type;			/* data type */
	char *	ctype;			/* C data type */
	char *	name;			/* field name */
	char *	elemc;			/* element count */
} ddlfld_t;

/* global data declarations */
extern char *	progname;		/* program name */
extern FILE *	yyin;			/* lex input stream */
extern FILE *	yyout;			/* lex output stream */
extern int	yylineno;		/* lex line number */
extern FILE *	hfp;			/* .h file stream */
extern FILE *	ifp;			/* .i file stream */
extern char *	recname;		/* record name */
extern int	dfc;			/* data file count */
extern ddldf_t *ddldfv;			/* ddl data file table */
extern int	ifc;			/* index file count */
extern ddlif_t *ddlifv;			/* ddl index file table */
extern int	fldc;			/* field count */
extern ddlfld_t *ddlfldv;		/* ddl field table */

/* function declarations */
#ifdef AC_PROTO
char *	ctype(const char *type);
int	ddlputh(FILE *fp, const char *recname, int fldc, const ddlfld_t fldv[]);
int	ddlputi(FILE *fp, const char *recname, int fldc, const ddlfld_t fldv[]);
int	fpctype(FILE *fp, const char *type);
bool	isalloc(const char *ctype);
char *	tmpupr(const char *s);
void	warning(const char *s, const char *t);
int	yyerror(const char *s);
int	yyparse(void);
#else
char *	ctype();
int	ddlputh();
int	ddlputi();
int	fpctype();
bool	isalloc();
char *	tmpupr();
void	warning();
int	yyerror();
int	yyparse();
#endif

#endif	/* #ifndef H_CBDDLP */
