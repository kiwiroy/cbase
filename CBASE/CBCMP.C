/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cbcmp.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <ctype.h>
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#ifdef AC_STRING
#include <string.h>
#endif

/* library headers */
#include <blkio.h>

/* local headers */
#include "cbase_.h"

/*man---------------------------------------------------------------------------
NAME
     cbcmp - cbase comparison functions

SYNOPSIS

DESCRIPTION

SEE ALSO
     cbexp, cbimp.

------------------------------------------------------------------------------*/
#define vcmp(CMPFCT) {							\
	int i = 0;							\
	int c = 0;							\
	int elems = n / sizeof(*cp1);					\
									\
	for (i = 0; i < elems; ++i) {					\
		c = CMPFCT(cp1, cp2, sizeof(*cp1));			\
		if (c != 0) return c;					\
		++cp1;							\
		++cp2;							\
	}								\
	return 0;							\
}

/* charcmp:  t_char comparison function */
#ifdef AC_PROTO
static int charcmp(const void *p1, const void *p2, size_t n)
#else
static int charcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	signed char n1 = '\0';
	signed char n2 = '\0';

	memcpy(&n1, p1, sizeof(n1));
	memcpy(&n2, p2, sizeof(n2));
	if (n1 < n2) {
		return -1;
	} else if (n1 > n2) {
		return 1;
	}

	return 0;
}

/* charvcmp:  t_charv comparison function */
#ifdef AC_PROTO
static int charvcmp(const void *p1, const void *p2, size_t n)
#else
static int charvcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	signed char *cp1 = (signed char *)p1;
	signed char *cp2 = (signed char *)p2;

	vcmp(charcmp);
}

/* ucharcmp:  t_uchar comparison function */
#ifdef AC_PROTO
static int ucharcmp(const void *p1, const void *p2, size_t n)
#else
static int ucharcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	unsigned char n1 = '\0';
	unsigned char n2 = '\0';

	memcpy(&n1, p1, sizeof(n1));
	memcpy(&n2, p2, sizeof(n2));
	if (n1 < n2) {
		return -1;
	} else if (n1 > n2) {
		return 1;
	}

	return 0;
}

/* ucharvcmp:  t_ucharv comparison function */
#ifdef AC_PROTO
static int ucharvcmp(const void *p1, const void *p2, size_t n)
#else
static int ucharvcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	unsigned char *cp1 = (unsigned char *)p1;
	unsigned char *cp2 = (unsigned char *)p2;

	vcmp(ucharcmp);
}

/* shortcmp:  t_short comparison function */
#ifdef AC_PROTO
static int shortcmp(const void *p1, const void *p2, size_t n)
#else
static int shortcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	signed short n1 = 0;
	signed short n2 = 0;

	memcpy(&n1, p1, sizeof(n1));
	memcpy(&n2, p2, sizeof(n2));
	if (n1 < n2) {
		return -1;
	} else if (n1 > n2) {
		return 1;
	}

	return 0;
}

/* shortvcmp:  t_shortv comparison function */
#ifdef AC_PROTO
static int shortvcmp(const void *p1, const void *p2, size_t n)
#else
static int shortvcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	signed short *cp1 = (signed short *)p1;
	signed short *cp2 = (signed short *)p2;

	vcmp(shortcmp);
}

/* ushortcmp:  t_ushort comparison function */
#ifdef AC_PROTO
static int ushortcmp(const void *p1, const void *p2, size_t n)
#else
static int ushortcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	unsigned short n1 = 0;
	unsigned short n2 = 0;

	memcpy(&n1, p1, sizeof(n1));
	memcpy(&n2, p2, sizeof(n2));
	if (n1 < n2) {
		return -1;
	} else if (n1 > n2) {
		return 1;
	}

	return 0;
}

/* ushortvcmp:  t_ushortv comparison function */
#ifdef AC_PROTO
static int ushortvcmp(const void *p1, const void *p2, size_t n)
#else
static int ushortvcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	unsigned short *cp1 = (unsigned short *)p1;
	unsigned short *cp2 = (unsigned short *)p2;

	vcmp(ushortcmp);
}

/* intcmp:  t_int comparison function */
#ifdef AC_PROTO
static int intcmp(const void *p1, const void *p2, size_t n)
#else
static int intcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	signed int n1 = 0;
	signed int n2 = 0;

	memcpy(&n1, p1, sizeof(n1));
	memcpy(&n2, p2, sizeof(n2));
	if (n1 < n2) {
		return -1;
	} else if (n1 > n2) {
		return 1;
	}

	return 0;
}

/* intvcmp:  t_intv comparison function */
#ifdef AC_PROTO
static int intvcmp(const void *p1, const void *p2, size_t n)
#else
static int intvcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	signed int *cp1 = (signed int *)p1;
	signed int *cp2 = (signed int *)p2;

	vcmp(intcmp);
}

/* uintcmp:  t_uint comparison function */
#ifdef AC_PROTO
static int uintcmp(const void *p1, const void *p2, size_t n)
#else
static int uintcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	unsigned int n1 = 0;
	unsigned int n2 = 0;

	memcpy(&n1, p1, sizeof(n1));
	memcpy(&n2, p2, sizeof(n2));
	if (n1 < n2) {
		return -1;
	} else if (n1 > n2) {
		return 1;
	}

	return 0;
}

/* uintvcmp:  t_uintv comparison function */
#ifdef AC_PROTO
static int uintvcmp(const void *p1, const void *p2, size_t n)
#else
static int uintvcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	unsigned int *cp1 = (unsigned int *)p1;
	unsigned int *cp2 = (unsigned int *)p2;

	vcmp(uintcmp);
}

/* longcmp:  t_long comparison function */
#ifdef AC_PROTO
static int longcmp(const void *p1, const void *p2, size_t n)
#else
static int longcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	signed long n1 = 0;
	signed long n2 = 0;

	memcpy(&n1, p1, sizeof(n1));
	memcpy(&n2, p2, sizeof(n2));
	if (n1 < n2) {
		return -1;
	} else if (n1 > n2) {
		return 1;
	}

	return 0;
}

/* longvcmp:  t_longv comparison function */
#ifdef AC_PROTO
static int longvcmp(const void *p1, const void *p2, size_t n)
#else
static int longvcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	signed long *cp1 = (signed long *)p1;
	signed long *cp2 = (signed long *)p2;

	vcmp(longcmp);
}

/* ulongcmp:  t_ulong comparison function */
#ifdef AC_PROTO
static int ulongcmp(const void *p1, const void *p2, size_t n)
#else
static int ulongcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	unsigned long n1 = 0;
	unsigned long n2 = 0;

	memcpy(&n1, p1, sizeof(n1));
	memcpy(&n2, p2, sizeof(n2));
	if (n1 < n2) {
		return -1;
	} else if (n1 > n2) {
		return 1;
	}

	return 0;
}

/* ulongvcmp:  t_ulongv comparison function */
#ifdef AC_PROTO
static int ulongvcmp(const void *p1, const void *p2, size_t n)
#else
static int ulongvcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	unsigned long *cp1 = (unsigned long *)p1;
	unsigned long *cp2 = (unsigned long *)p2;

	vcmp(ulongcmp);
}

/* floatcmp:  t_float comparison function */
#ifdef AC_PROTO
static int floatcmp(const void *p1, const void *p2, size_t n)
#else
static int floatcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	float n1 = 0;
	float n2 = 0;

	memcpy(&n1, p1, sizeof(n1));
	memcpy(&n2, p2, sizeof(n2));
	if (n1 < n2) {
		return -1;
	} else if (n1 > n2) {
		return 1;
	}

	return 0;
}

/* floatvcmp:  t_floatv comparison function */
#ifdef AC_PROTO
static int floatvcmp(const void *p1, const void *p2, size_t n)
#else
static int floatvcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	float *cp1 = (float *)p1;
	float *cp2 = (float *)p2;

	vcmp(floatcmp);
}

/* dblcmp:  t_double comparison function */
#ifdef AC_PROTO
static int dblcmp(const void *p1, const void *p2, size_t n)
#else
static int dblcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	double n1 = 0;
	double n2 = 0;

	memcpy(&n1, p1, sizeof(n1));
	memcpy(&n2, p2, sizeof(n2));
	if (n1 < n2) {
		return -1;
	} else if (n1 > n2) {
		return 1;
	}

	return 0;
}

/* dblvcmp:  t_doublev comparison function */
#ifdef AC_PROTO
static int dblvcmp(const void *p1, const void *p2, size_t n)
#else
static int dblvcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	double *cp1 = (double *)p1;
	double *cp2 = (double *)p2;

	vcmp(dblcmp);
}

/* ldblcmp:  t_ldouble comparison function */
#ifdef AC_PROTO
static int ldblcmp(const void *p1, const void *p2, size_t n)
#else
static int ldblcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
#ifdef AC_LDOUBLE
	long double n1 = 0;
	long double n2 = 0;

	memcpy(&n1, p1, sizeof(n1));
	memcpy(&n2, p2, sizeof(n2));
	if (n1 < n2) {
		return -1;
	} else if (n1 > n2) {
		return 1;
	}

	return 0;
#else
	return 0;
#endif
}

/* ldblvcmp:  t_ldoublev comparison function */
#ifdef AC_PROTO
static int ldblvcmp(const void *p1, const void *p2, size_t n)
#else
static int ldblvcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
#ifdef AC_LDOUBLE
	long double *cp1 = (long double *)p1;
	long double *cp2 = (long double *)p2;

	vcmp(ldblcmp);
#else
	return 0;
#endif
}

/* ptrcmp:  t_pointer comparison function */
#ifdef AC_PROTO
static int ptrcmp(const void *p1, const void *p2, size_t n)
#else
static int ptrcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	void *n1 = NULL;
	void *n2 = NULL;

	memcpy(&n1, p1, sizeof(n1));
	memcpy(&n2, p2, sizeof(n2));
	if (n1 < n2) {
		return -1;
	} else if (n1 > n2) {
		return 1;
	}

	return 0;
}

/* ptrvcmp:  t_pointerv comparison function */
#ifdef AC_PROTO
static int ptrvcmp(const void *p1, const void *p2, size_t n)
#else
static int ptrvcmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	void **cp1 = (void **)p1;
	void **cp2 = (void **)p2;

	vcmp(ptrcmp);
}

/* t_string -> use standard library function strncmp */

/* cistrncmp:  t_cistring comparison function */
#ifdef AC_PROTO
static int cistrncmp(const char *cs1, const char *cs2, size_t n)
#else
static int cistrncmp(cs1, cs2, n)
const char *cs1;
const char *cs2;
size_t n;
#endif
{
	int i = 0;	/* loop counter */

	for (i = 0; i < n && toupper(*cs1) == toupper(*cs2); ++i, ++cs1, ++cs2) {
		if (*cs1 == '\0') {
			return 0;
		}
	}

	return toupper(*cs1) - toupper(*cs2);
}

/* bincmp:  t_binary comparison function */
#ifdef AC_PROTO
static int bincmp(const void *p1, const void *p2, size_t n)
#else
static int bincmp(p1, p2, n)
const void *p1;
const void *p2;
size_t n;
#endif
{
	unsigned char *v1 = (unsigned char *)p1;
	unsigned char *v2 = (unsigned char *)p2;

	for (; n > 0; --n, ++v1, ++v2) {
		if (*v1 != *v2) {
			return *v1 - *v2;
		}
	}

	return 0;
}

/* cbase comparison function table definition */
const cbcmp_t cbcmpv[] = {
	charcmp,		/* t_char	=  0 */
	charvcmp,		/* t_charv	=  1 */
	ucharcmp,		/* t_uchar	=  2 */
	ucharvcmp,		/* t_ucharv	=  3 */
	shortcmp,		/* t_short	=  4 */
	shortvcmp,		/* t_shortv	=  5 */
	ushortcmp,		/* t_ushort	=  6 */
	ushortvcmp,		/* t_ushortv	=  7 */
	intcmp,			/* t_int	=  8 */
	intvcmp,		/* t_intv	=  9 */
	uintcmp,		/* t_uint	= 10 */
	uintvcmp,		/* t_uintv	= 11 */
	longcmp,		/* t_long	= 12 */
	longvcmp,		/* t_longv	= 13 */
	ulongcmp,		/* t_ulong	= 14 */
	ulongvcmp,		/* t_ulongv	= 15 */
	floatcmp,		/* t_float	= 16 */
	floatvcmp,		/* t_floatv	= 17 */
	dblcmp,			/* t_double	= 18 */
	dblvcmp,		/* t_doublev	= 19 */
	ldblcmp,		/* t_ldouble	= 20 */
	ldblvcmp,		/* t_ldoublev	= 21 */
	ptrcmp,			/* t_pointer	= 22 */
	ptrvcmp,		/* t_pointerv	= 23 */
	(cbcmp_t)strncmp,	/* t_string	= 24 */
	(cbcmp_t)cistrncmp,	/* t_cistring	= 25 */
	bincmp,			/* t_binary	= 26 */
};
