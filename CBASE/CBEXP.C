/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbexp.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#include <stdio.h>
#ifdef AC_STRING
#include <string.h>
#endif

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cbexp - cbase export functions

SYNOPSIS

DESCRIPTION

SEE ALSO
     cbcmp, cbimp.

DIAGNOSTICS

WARNINGS
     Pointer format is implementation defined; it could possibly
     contain the field delimiter ('|') or escape ('\\\\') chars.  If
     so, the field delimiter or escape character will have to be
     changed.

------------------------------------------------------------------------------*/
/* array data type export macro */
#define vexp(EXPFCT) {							\
	int i = 0;							\
	int elems = n / sizeof(*cp);					\
									\
	for (i = 0; i < elems; ++i) {					\
		if (EXPFCT(fp, cp, sizeof(*cp)) == -1) return -1;	\
		if (i < elems - 1) {					\
			if (fputc(EXPFLDDLM, fp) == EOF) return -1;	\
		}							\
		++cp;							\
	}								\
	return 0;							\
}

/* t_char -> use t_uchar export function */
#define charexp		ucharexp

/* t_charv -> use t_ucharv export function */
#define charvexp	ucharvexp

/* ucharexp:  t_uchar export function */
#ifdef AC_PROTO
static int ucharexp(FILE *fp, const void *p, size_t n)
#else
static int ucharexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	unsigned char c = *(unsigned char *)p;

	switch (c) {
	case EXPFLDDLM:	/* export file field delimiter */
		if (fputc(EXPESC, fp) == EOF) return -1;
		if (fputc('F', fp) == EOF) return -1;
		break;	/* case EXPFLDDLM: */
	case EXPESC:	/* export file field escape character */
		if (fputc(EXPESC, fp) == EOF) return -1;
		if (fputc(EXPESC, fp) == EOF) return -1;
		break;	/* case EXPESC: */
#ifdef AC_ESCAPE
	case '\a':	/* audible alert */
		if (fputc(EXPESC, fp) == EOF) return -1;
		if (fputc('a', fp) == EOF) return -1;
		break;	/* case '\a': */
#endif
	case '\b':	/* backspace */
		if (fputc(EXPESC, fp) == EOF) return -1;
		if (fputc('b', fp) == EOF) return -1;
		break;	/* case '\b': */
	case '\f':	/* form feed */
		if (fputc(EXPESC, fp) == EOF) return -1;
		if (fputc('f', fp) == EOF) return -1;
		break;	/* case '\f': */
	case '\n':	/* newline */
		if (fputc(EXPESC, fp) == EOF) return -1;
		if (fputc('n', fp) == EOF) return -1;
		break;	/* case '\n': */
	case '\r':	/* carriage return */
		if (fputc(EXPESC, fp) == EOF) return -1;
		if (fputc('r', fp) == EOF) return -1;
		break;	/* case '\r': */
	case '\t':	/* horizontal tab */
		if (fputc(EXPESC, fp) == EOF) return -1;
		if (fputc('t', fp) == EOF) return -1;
		break;	/* case '\t': */
	case '\v':	/* vertical tab */
		if (fputc(EXPESC, fp) == EOF) return -1;
		if (fputc('v', fp) == EOF) return -1;
		break;	/* case '\v': */
	default:
		if (isprint(c)) {
			if (fputc(c, fp) == EOF) return -1;
		} else {
			if (fprintf(fp, "%c%.3o", (int)EXPESC, (int)c) < 0) {
				return -1;
			}
		}
	}

	return 0;
}

/* ucharvexp:  t_ucharv export function */
#ifdef AC_PROTO
static int ucharvexp(FILE *fp, const void *p, size_t n)
#else
static int ucharvexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	unsigned char *cp = (unsigned char *)p;

	vexp(ucharexp);
}

/* shortexp:  t_short export function */
#ifdef AC_PROTO
static int shortexp(FILE *fp, const void *p, size_t n)
#else
static int shortexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	signed short x = 0;

	memcpy(&x, p, sizeof(x));
	if (fprintf(fp, "%hd", (int)x) < 0) return -1;

	return 0;
}

/* shortvexp:  t_shortv export function */
#ifdef AC_PROTO
static int shortvexp(FILE *fp, const void *p, size_t n)
#else
static int shortvexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	signed short *cp = (signed short *)p;

	vexp(shortexp);
}

/* ushortexp:  t_ushort export function */
#ifdef AC_PROTO
static int ushortexp(FILE *fp, const void *p, size_t n)
#else
static int ushortexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	unsigned short x = 0;

	memcpy(&x, p, sizeof(x));
	if (fprintf(fp, "%hu", (unsigned int)x) < 0) return -1;

	return 0;
}

/* ushortvexp:  t_ushortv export function */
#ifdef AC_PROTO
static int ushortvexp(FILE *fp, const void *p, size_t n)
#else
static int ushortvexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	unsigned short *cp = (unsigned short *)p;

	vexp(ushortexp);
}

/* intexp:  t_int export function */
#ifdef AC_PROTO
static int intexp(FILE *fp, const void *p, size_t n)
#else
static int intexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	signed int x = 0;

	memcpy(&x, p, sizeof(x));
	if (fprintf(fp, "%d", x) < 0) return -1;

	return 0;
}

/* intvexp:  t_intv export function */
#ifdef AC_PROTO
static int intvexp(FILE *fp, const void *p, size_t n)
#else
static int intvexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	signed int *cp = (signed int *)p;

	vexp(intexp);
}

/* uintexp:  t_uint export function */
#ifdef AC_PROTO
static int uintexp(FILE *fp, const void *p, size_t n)
#else
static int uintexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	unsigned int x = 0;

	memcpy(&x, p, sizeof(x));
	if (fprintf(fp, "%u", x) < 0) return -1;

	return 0;
}

/* uintvexp:  t_uintv export function */
#ifdef AC_PROTO
static int uintvexp(FILE *fp, const void *p, size_t n)
#else
static int uintvexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	unsigned int *cp = (unsigned int *)p;

	vexp(uintexp);
}

/* longexp:  t_long export function */
#ifdef AC_PROTO
static int longexp(FILE *fp, const void *p, size_t n)
#else
static int longexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	signed long x = 0;

	memcpy(&x, p, sizeof(x));
	if (fprintf(fp, "%ld", x) < 0) return -1;

	return 0;
}

/* longvexp:  t_longv export function */
#ifdef AC_PROTO
static int longvexp(FILE *fp, const void *p, size_t n)
#else
static int longvexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	signed long *cp = (signed long *)p;

	vexp(longexp);
}

/* ulongexp:  t_ulong export function */
#ifdef AC_PROTO
static int ulongexp(FILE *fp, const void *p, size_t n)
#else
static int ulongexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	unsigned long x = 0;

	memcpy(&x, p, sizeof(x));
	if (fprintf(fp, "%lu", x) < 0) return -1;

	return 0;
}

/* ulongvexp:  t_ulongv export function */
#ifdef AC_PROTO
static int ulongvexp(FILE *fp, const void *p, size_t n)
#else
static int ulongvexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	unsigned long *cp = (unsigned long *)p;

	vexp(ulongexp);
}

/* floatexp:  t_float export function */
#ifdef AC_PROTO
static int floatexp(FILE *fp, const void *p, size_t n)
#else
static int floatexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	float x = 0;

	memcpy(&x, p, sizeof(x));
	if (fprintf(fp, "%G", (double)x) < 0) return -1;

	return 0;
}

/* floatvexp:  t_floatv export function */
#ifdef AC_PROTO
static int floatvexp(FILE *fp, const void *p, size_t n)
#else
static int floatvexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	float *cp = (float *)p;

	vexp(floatexp);
}

/* dblexp:  t_double export function */
#ifdef AC_PROTO
static int dblexp(FILE *fp, const void *p, size_t n)
#else
static int dblexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	double x = 0;

	memcpy(&x, p, sizeof(x));
	if (fprintf(fp, "%G", x) < 0) return -1;

	return 0;
}

/* dblexp:  t_double export function */
#ifdef AC_PROTO
static int dblvexp(FILE *fp, const void *p, size_t n)
#else
static int dblvexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	double *cp = (double *)p;

	vexp(dblexp);
}

/* ldblexp:  t_ldouble export function */
#ifdef AC_PROTO
static int ldblexp(FILE *fp, const void *p, size_t n)
#else
static int ldblexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
#ifdef AC_LDOUBLE
	long double x = 0;

	memcpy(&x, p, sizeof(x));
	if (fprintf(fp, "%LG", x) < 0) return -1;

	return 0;
#else
	return -1;
#endif
}

/* ldblvexp:  t_ldoublev export function */
#ifdef AC_PROTO
static int ldblvexp(FILE *fp, const void *p, size_t n)
#else
static int ldblvexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
#ifdef AC_LDOUBLE
	long double *cp = (long double *)p;

	vexp(ldblexp);
#else
	return -1;
#endif
}

/* ptrexp:  t_pointer export function */
#ifdef AC_PROTO
static int ptrexp(FILE *fp, const void *p, size_t n)
#else
static int ptrexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	void *x = NULL;

	memcpy(&x, p, sizeof(x));
	if (fprintf(fp, "%p", x) < 0) return -1;

	return 0;
}

/* ptrvexp:  t_pointerv export function */
#ifdef AC_PROTO
static int ptrvexp(FILE *fp, const void *p, size_t n)
#else
static int ptrvexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	void **cp = (void **)p;

	vexp(ptrexp);
}

/* strexp:  t_string export function */
#ifdef AC_PROTO
static int strexp(FILE *fp, const void *p, size_t n)
#else
static int strexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	char *s = (char *)p;
	int i = 0;

	for (i = 0; i < n; ++i) {
		if (s[i] == NUL) {
			break;
		}
		if (ucharexp(fp, &s[i], sizeof(*s)) == -1) {
			return -1;
		}
	}

	return 0;
}

/* t_cistring -> use t_string export function */
#define cistrexp	strexp

/* binexp:  t_binary export function */
#ifdef AC_PROTO
static int binexp(FILE *fp, const void *p, size_t n)
#else
static int binexp(fp, p, n)
FILE *fp;
const void *p;
size_t n;
#endif
{
	/* calculate number of hex digits for each char
	     # bits in char == CHAR_BIT, # bits in hex digit == 4 */
	const int hexdigits = (CHAR_BIT + (4 - 1)) / 4;
	unsigned char *pi = (unsigned char *)p;
	char fmt[24];			/* printf format string */

	sprintf(fmt, "%%.%dX", hexdigits);
	while (pi < ((unsigned char *)p + n)) {
		if (fprintf(fp, fmt, (unsigned int)*pi++) < 0) return -1;
	}

	return 0;
}

/* export function table definition */
const cbexp_t cbexpv[] = {
	charexp,		/* t_char	=  0 */
	charvexp,		/* t_charv	=  1 */
	ucharexp,		/* t_uchar	=  2 */
	ucharvexp,		/* t_ucharv	=  3 */
	shortexp,		/* t_short	=  4 */
	shortvexp,		/* t_shortv	=  5 */
	ushortexp,		/* t_ushort	=  6 */
	ushortvexp,		/* t_ushortv	=  7 */
	intexp,			/* t_int	=  8 */
	intvexp,		/* t_intv	=  9 */
	uintexp,		/* t_uint	= 10 */
	uintvexp,		/* t_uintv	= 11 */
	longexp,		/* t_long	= 12 */
	longvexp,		/* t_longv	= 13 */
	ulongexp,		/* t_ulong	= 14 */
	ulongvexp,		/* t_ulongv	= 15 */
	floatexp,		/* t_float	= 16 */
	floatvexp,		/* t_floatv	= 17 */
	dblexp,			/* t_double	= 18 */
	dblvexp,		/* t_doublev	= 19 */
	ldblexp,		/* t_ldouble	= 20 */
	ldblvexp,		/* t_ldoublev	= 21 */
	ptrexp,			/* t_pointer	= 22 */
	ptrvexp,		/* t_pointerv	= 23 */
	strexp,			/* t_string	= 24 */
	cistrexp,		/* t_cistring	= 25 */
	binexp,			/* t_binary	= 26 */
};
