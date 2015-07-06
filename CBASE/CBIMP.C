/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbimp.c	1.5 - 91/09/23" */

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

#ifndef isodigit
#define isodigit(c)	(((c) >= '0' && (c) <= '7') ? 1 : 0)
#endif

/*man---------------------------------------------------------------------------
NAME
     cbimp - import cbase data

SYNOPSIS

DESCRIPTION

SEE ALSO
     cbcmp, cbexp.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
/* array data type import macro */
#define vimp(IMPFCT) {							\
	int i = 0;							\
	int elems = n / sizeof(*cp);					\
									\
	for (i = 0; i < elems; ++i) {					\
		if (IMPFCT(fp, cp, sizeof(*cp)) == -1) return -1;	\
		++cp;							\
	}								\
	return 0;							\
}

#define MAXFLDLEN	(1024)
static char s[MAXFLDLEN + 1];

/* getfld:  get next field from import file */
#ifdef AC_PROTO
static int getfld(FILE *fp)
#else
static int getfld(fp)
FILE *fp;
#endif
{
	int i = 0;
	int c = 0;

	memset(s, 0, sizeof(s));
	for (i = 0; ; ++i) {
		c = fgetc(fp);
		if (ferror(fp)) {
			CBEPRINT;
			return -1;
		}
		if (feof(fp)) {
			CBEPRINT;
			return -1;
		}
		if (c == EXPFLDDLM || c == EXPRECDLM) {
			break;
		}
		if (i >= sizeof(s) - 1) {
			CBEPRINT;
			return -1;
		}
		s[i] = c;
	}
	s[i] = NUL;

	return 0;
}

/* t_char -> use t_uchar import function */
#define charimp		ucharimp

/* t_charv -> use t_ucharv import function */
#define charvimp	ucharvimp

/* ucharimp:  t_uchar import function */
#ifdef AC_PROTO
static int ucharimp(FILE *fp, void *p, size_t n)
#else
static int ucharimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	int c = 0;

	if (getfld(fp) == -1) {
		return -1;
	}
	c = (unsigned char)s[0];
	if (c == EXPESC) {
		switch ((unsigned char)s[1]) {
		case NUL:	/* premature end of field */
			return -1;
			break;	/* case NUL: */
#ifdef AC_BELL
		case 'a':	/* audible alert */
			if (s[2] != NUL) return -1;
			c = '\a';
			break;	/* case 'a': */
#endif
		case 'b':	/* backspace */
			if (s[2] != NUL) return -1;
			c = '\b';
			break;	/* case 'b': */
		case 'f':	/* form feed */
			if (s[2] != NUL) return -1;
			c = '\f';
			break;	/* case 'f': */
		case 'n':	/* newline */
			if (s[2] != NUL) return -1;
			c = '\n';
			break;	/* case 'n': */
		case 'r':	/* carriage return */
			if (s[2] != NUL) return -1;
			c = '\r';
			break;	/* case 'r': */
		case 't':	/* horizontal tab */
			if (s[2] != NUL) return -1;
			c = '\t';
			break;	/* case 't': */
		case 'v':	/* vertical tab */
			if (s[2] != NUL) return -1;
			c = '\v';
			break;	/* case 'v': */
		default:	/* */
			if (isodigit(s[1])) {
				if (!isodigit(s[2]) || !isodigit(s[3])) return -1;
				if (s[4] != NUL) return -1;
				if (sscanf(s + 1, "%3o", &c) != 1) return -1;
			} else {
				if (s[2] != NUL) return -1;
				c = (unsigned char)s[1];
			}
			break;	/* default: */
		}
	}
	*(unsigned char *)p = c;

	return 0;
}

/* ucharvimp:  t_ucharv import function */
#ifdef AC_PROTO
static int ucharvimp(FILE *fp, void *p, size_t n)
#else
static int ucharvimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	unsigned char *cp = (unsigned char *)p;

	vimp(ucharimp);
}

/* shortimp:  t_short import function */
#ifdef AC_PROTO
static int shortimp(FILE *fp, void *p, size_t n)
#else
static int shortimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	if (getfld(fp) == -1) {
		return -1;
	}
	if (s[0] == NUL) {
		*(short *)p = 0;
	} else {
		if (sscanf(s, "%hd", (short *)p) != 1) return -1;
	}

	return 0;
}

/* shortvimp:  t_shortv import function */
#ifdef AC_PROTO
static int shortvimp(FILE *fp, void *p, size_t n)
#else
static int shortvimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	signed short *cp = (signed short *)p;

	vimp(shortimp);
}

/* ushortimp:  t_ushort import function */
#ifdef AC_PROTO
static int ushortimp(FILE *fp, void *p, size_t n)
#else
static int ushortimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	if (getfld(fp) == -1) {
		return -1;
	}
	if (s[0] == NUL) {
		*(unsigned short *)p = 0;
	} else {
		if (sscanf(s, "%hu", (unsigned short *)p) != 1) return -1;
	}

	return 0;
}

/* ushortvimp:  t_ushortv import function */
#ifdef AC_PROTO
static int ushortvimp(FILE *fp, void *p, size_t n)
#else
static int ushortvimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	unsigned short *cp = (unsigned short *)p;

	vimp(ushortimp);
}

/* intimp:  t_int import function */
#ifdef AC_PROTO
static int intimp(FILE *fp, void *p, size_t n)
#else
static int intimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	if (getfld(fp) == -1) {
		return -1;
	}
	if (s[0] == NUL) {
		*(int *)p = 0;
	} else {
		if (sscanf(s, "%d", (int *)p) != 1) return -1;
	}

	return 0;
}

/* intvimp:  t_intv import function */
#ifdef AC_PROTO
static int intvimp(FILE *fp, void *p, size_t n)
#else
static int intvimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	signed int *cp = (signed int *)p;

	vimp(intimp);
}

/* uintimp:  t_uint import function */
#ifdef AC_PROTO
static int uintimp(FILE *fp, void *p, size_t n)
#else
static int uintimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	if (getfld(fp) == -1) {
		return -1;
	}
	if (s[0] == NUL) {
		*(unsigned int *)p = 0;
	} else {
		if (sscanf(s, "%u", (unsigned int *)p) != 1) return -1;
	}

	return 0;
}

/* uintvimp:  t_uintv import function */
#ifdef AC_PROTO
static int uintvimp(FILE *fp, void *p, size_t n)
#else
static int uintvimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	unsigned int *cp = (unsigned int *)p;

	vimp(uintimp);
}

/* longimp:  t_long import function */
#ifdef AC_PROTO
static int longimp(FILE *fp, void *p, size_t n)
#else
static int longimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	if (getfld(fp) == -1) {
		return -1;
	}
	if (s[0] == NUL) {
		*(long *)p = 0;
	} else {
		if (sscanf(s, "%ld", (long *)p) != 1) return -1;
	}

	return 0;
}

/* longvimp:  t_longv import function */
#ifdef AC_PROTO
static int longvimp(FILE *fp, void *p, size_t n)
#else
static int longvimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	signed long *cp = (signed long *)p;

	vimp(longimp);
}

/* ulongimp:  t_ulong import function */
#ifdef AC_PROTO
static int ulongimp(FILE *fp, void *p, size_t n)
#else
static int ulongimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	if (getfld(fp) == -1) {
		return -1;
	}
	if (s[0] == NUL) {
		*(unsigned long *)p = 0;
	} else {
		if (sscanf(s, "%lu", (unsigned long *)p) != 1) return -1;
	}

	return 0;
}

/* ulongvimp:  t_ulongv import function */
#ifdef AC_PROTO
static int ulongvimp(FILE *fp, void *p, size_t n)
#else
static int ulongvimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	unsigned long *cp = (unsigned long *)p;

	vimp(ulongimp);
}

/* floatimp:  t_float import function */
#ifdef AC_PROTO
static int floatimp(FILE *fp, void *p, size_t n)
#else
static int floatimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	if (getfld(fp) == -1) {
		return -1;
	}
	if (s[0] == NUL) {
		*(float *)p = 0;
	} else {
		if (sscanf(s, "%g", (float *)p) != 1) return -1;
	}

	return 0;
}

/* floatvimp:  t_floatv import function */
#ifdef AC_PROTO
static int floatvimp(FILE *fp, void *p, size_t n)
#else
static int floatvimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	float *cp = (float *)p;

	vimp(floatimp);
}

/* dblimp:  t_ldouble import function */
#ifdef AC_PROTO
static int dblimp(FILE *fp, void *p, size_t n)
#else
static int dblimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	if (getfld(fp) == -1) {
		return -1;
	}
	if (s[0] == NUL) {
		*(double *)p = 0;
	} else {
		if (sscanf(s, "%lg", (double *)p) != 1) return -1;
	}

	return 0;
}

/* dblvimp:  t_doublev import function */
#ifdef AC_PROTO
static int dblvimp(FILE *fp, void *p, size_t n)
#else
static int dblvimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	double *cp = (double *)p;

	vimp(dblimp);
}

/* ldblimp:  t_ldouble import function */
#ifdef AC_PROTO
static int ldblimp(FILE *fp, void *p, size_t n)
#else
static int ldblimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	if (getfld(fp) == -1) {
		return -1;
	}
#ifdef AC_LDOUBLE
	if (s[0] == NUL) {
		*(long double *)p = 0;
	} else {
		if (sscanf(s, "%Lg", (long double *)p) != 1) return -1;
	}
#endif
	return 0;
}

/* ldblimp:  t_ldouble import function */
#ifdef AC_PROTO
static int ldblvimp(FILE *fp, void *p, size_t n)
#else
static int ldblvimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
#ifdef AC_LDOUBLE
	long double *cp = (long double *)p;
#else
	double *cp = (double *)p;
#endif
	vimp(ldblimp);
}

/* ptrimp:  t_pointer import function */
#ifdef AC_PROTO
static int ptrimp(FILE *fp, void *p, size_t n)
#else
static int ptrimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	if (getfld(fp) == -1) {
		return -1;
	}
	if (s[0] == NUL) {
		*(void **)p = NULL;
	} else {
		if (sscanf(s, "%p", (void **)p) != 1) return -1;
	}

	return 0;
}

/* ptrvimp:  t_pointerv import function */
#ifdef AC_PROTO
static int ptrvimp(FILE *fp, void *p, size_t n)
#else
static int ptrvimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	void **cp = (void **)p;

	vimp(ptrimp);
}

/* strimp:  t_string import function */
#ifdef AC_PROTO
static int strimp(FILE *fp, void *p, size_t n)
#else
static int strimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	unsigned char *si = NULL;
	int i = 0;
	int c = 0;

	if (getfld(fp) == -1) {
		return -1;
	}

	/* initialize return */
	memset(p, 0, n);

	/* convert */ /* string will be truncated if too long */
	si = (unsigned char *)s;
	for (i = 0; i < n; ++i) {
		c = *si++;
		if (c == NUL) {
			*((char *)p + i) = NUL;
			break;
		}
		if (c == EXPESC) {
			switch (*si) {
			case NUL:	/* premature end of field */
				return -1;
				break;	/* case NUL: */
#ifdef AC_BELL
			case 'a':	/* audible alert */
				c = '\a';
				break;	/* case 'a': */
#endif
			case 'b':	/* backspace */
				c = '\b';
				break;	/* case 'b': */
			case 'f':	/* form feed */
				c = '\f';
				break;	/* case 'f': */
			case 'n':	/* newline */
				c = '\n';
				break;	/* case 'n': */
			case 'r':	/* carriage return */
				c = '\r';
				break;	/* case 'r': */
			case 't':	/* horizontal tab */
				c = '\t';
				break;	/* case 't': */
			case 'v':	/* vertical tab */
				c = '\v';
				break;	/* case 'v': */
			default:	/* */
				if (isodigit(*si)) {
					if (!isodigit(*(si + 1)) || !isodigit(*(si + 2))) return -1;
					if (sscanf(si, "%3o", &c) != 1) return -1;
					si += 2;
				} else {
					c = *si;
				}
				break;	/* default: */
			}
			++si;
		}
		*((char *)p + i) = c;
	}

	return 0;
}

/* t_cistring -> use t_string import function */
#define cistrimp	strimp

/* binimp:  t_binary import function */
#ifdef AC_PROTO
static int binimp(FILE *fp, void *p, size_t n)
#else
static int binimp(fp, p, n)
FILE *fp;
void *p;
size_t n;
#endif
{
	/* calculate number of hex digits for each char
	     # bits in char == CHAR_BIT, # bits in hex digit == 4 */
	const int hexdigits = (CHAR_BIT + (4 - 1)) / 4;
	unsigned char *si = (unsigned char *)s;
	unsigned char *pi = (unsigned char *)p;
	char fmt[24];			/* printf format string */
	int c = 0;

	sprintf(fmt, "%%%dX", hexdigits);

	if (getfld(fp) == -1) {
		return -1;
	}

	/* initialize return */
	memset(p, 0, n);

	for (;;) {
		if (si - (unsigned char *)s >= sizeof(s) - hexdigits) break;
		if (pi - (unsigned char *)p >= n) break;
		if (sscanf(si, fmt, &c) != 1) return -1;
		si += hexdigits;
		*pi++ = (unsigned char)c;
	}

	return 0;
}

/* import function table definition */
const cbimp_t cbimpv[] = {
	charimp,		/* t_char	=  0 */
	charvimp,		/* t_charv	=  1 */
	ucharimp,		/* t_uchar	=  2 */
	ucharvimp,		/* t_ucharv	=  3 */
	shortimp,		/* t_short	=  4 */
	shortvimp,		/* t_shortv	=  5 */
	ushortimp,		/* t_ushort	=  6 */
	ushortvimp,		/* t_ushortv	=  7 */
	intimp,			/* t_int	=  8 */
	intvimp,		/* t_intv	=  9 */
	uintimp,		/* t_uint	= 10 */
	uintvimp,		/* t_uintv	= 11 */
	longimp,		/* t_long	= 12 */
	longvimp,		/* t_longv	= 13 */
	ulongimp,		/* t_ulong	= 14 */
	ulongvimp,		/* t_ulongv	= 15 */
	floatimp,		/* t_float	= 16 */
	floatvimp,		/* t_floatv	= 17 */
	dblimp,			/* t_double	= 18 */
	dblvimp,		/* t_doublev	= 19 */
	ldblimp,		/* t_ldouble	= 20 */
	ldblvimp,		/* t_ldoublev	= 21 */
	ptrimp,			/* t_pointer	= 22 */
	ptrvimp,		/* t_pointerv	= 23 */
	strimp,			/* t_string	= 24 */
	cistrimp,		/* t_cistring	= 25 */
	binimp,			/* t_binary	= 26 */
};
