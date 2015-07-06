/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* definition section --------------------------------------------------------*/
%{
/* #ident	"@(#)cbddlpl.l	1.6 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#ifdef AC_STDLIB
#include <stdlib.h>
#endif

/* local headers */
#include "basstr.h"
#include "cbddlp.h"
#ifdef DOS			/* note:  The DOS patches are for GNU flex */
#include "y_tab.h"

#undef ECHO			/* disable lex output */
#define ECHO
int yylineno = 0;
#else
#include "y.tab.h"

#undef output			/* disable lex output */
#define output(c)
#endif

/* function declarations */
#ifdef AC_PROTO
int skipcomment(void);
#else
int skipcomment();
#endif

#ifdef DEBUG
#define DBPRINT {							\
	fprintf(stderr, "%s[%d]: \"%s\" found at line %d.\n",		\
				 __FILE__, __LINE__, yytext, yylineno);	\
}
#else
#define DBPRINT
#endif
%}
/* rule section --------------------------------------------------------------*/
%%
compound {			/* keyword compound */
	DBPRINT;
	return COMPOUND;
}
contains {			/* keyword contains */
	DBPRINT;
	return CONTAINS;
}
data[ \t\n]+file {		/* keyword data file */
	DBPRINT;
	return DATAFILE;
}
key {				/* keyword key */
	DBPRINT;
	return KEY;
}
index[ \t\n]+file {		/* keyword index file */
	DBPRINT;
	return INDEXFILE;
}
record {			/* keyword record */
	DBPRINT;
	return RECORD;
}
unique {			/* keyword unique */
	DBPRINT;
	return UNIQUE;
}
[ \t]+ {				/* blanks and tabs */
	DBPRINT;
}
[A-Za-z][A-Za-z0-9_]* {			/* identifier */
	DBPRINT;
	/* copy identifier to yylval */
	yylval.buf = (char *)malloc(yyleng + 1);
	if (yylval.buf == NULL) {
		perror("out of memory");
		exit(EXIT_FAILURE);
	}
	strncpy(yylval.buf, yytext, yyleng);
	yylval.buf[yyleng] = NUL;
	return IDENTIFIER;
}
^[ \t]*#.*$ {				/* C preprocessor statement */
	DBPRINT;
	fputs(yytext, hfp);
	fputc('\n', hfp);
}
\" {					/* string literal */
	for (;;++yyleng) {
		yytext[yyleng] = input();
		if (yytext[yyleng] == '\n') {
			yyerror("newline in string constant");
			exit(EXIT_FAILURE);
		}
		if (feof(yyin)) {
			yyerror("unexpected EOF in string constant");
			exit(EXIT_FAILURE);
		}
		if (yytext[yyleng] == '\"' && yytext[yyleng - 1] != '\\') {
			++yyleng;
			break;
		}
	}
	yytext[yyleng] = NUL;
	DBPRINT;
	/* copy string to yylval w/o enclosing quotes */
	yylval.buf = (char *)malloc(yyleng - 2 + 1);
	if (yylval.buf == NULL) {
		perror("out of memory");
		exit(EXIT_FAILURE);
	}
	strncpy(yylval.buf, yytext + 1, yyleng - 2);
	yylval.buf[yyleng - 2] = NUL; 
	return STRING;
}
\[ {					/* array element count */
	for (;;++yyleng) {
		yytext[yyleng] = input();
		if (feof(yyin)) {
			yyerror("unexpected EOF in string constant");
			exit(EXIT_FAILURE);
		}
		if (yytext[yyleng] == ']') {
			++yyleng;
			break;
		}
	}
	yytext[yyleng] = NUL;
	DBPRINT;
	/* copy text to yylval w/o enclosing brackets */
	yylval.buf = (char *)malloc(yyleng - 2 + 1);
	if (yylval.buf == NULL) {
		perror("out of memory");
		exit(EXIT_FAILURE);
	}
	strncpy(yylval.buf, yytext + 1, yyleng - 2);
	yylval.buf[yyleng - 2] = NUL;
	return ELEMC;
}
"/*" {					/* comment */
	DBPRINT;
	skipcomment();
}
. {					/* single character delimiter */
	DBPRINT;
	return *yytext;
}
%%

/* subroutine section --------------------------------------------------------*/
/* skipcomment:  skip C comment */
#ifdef AC_PROTO
int skipcomment(void) 
#else
int skipcomment() 
#endif
{
	int	c	= 0;

	for (;;) {
		while (!feof(yyin)) {
			c = input();
			if (c == '*') {
				break;
			}
		}
		if (feof(yyin)) {
			yyerror("unexpected EOF in comment");
			exit(EXIT_FAILURE);
		}
		if (input() == '/') {
			break;
		} else {
			unput(yytext[yyleng - 1]);
		}
	}

	return 0;
}
