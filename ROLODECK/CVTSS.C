/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)cvtss.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <ctype.h>
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

/* local headers */
#include "basstr.h"

/* macros */
/* maximum of two values */
#define max(a,b)	((a) > (b) ? (a) : (b))
/* minimum of two values */
#define min(a,b)	((a) < (b) ? (a) : (b))

/* macro to preserve characters within single and double quotes */
#define QUOTES {							\
	char c = NUL;							\
	if (quotes && (*ps == '\'' || *ps == '\"')) {			\
		c = *ps;						\
		*pt++ = *ps++;						\
		while (pt < ttend) {					\
			*pt++ = *ps;					\
			if (*ps == c || *ps == NUL) break;		\
		}							\
		continue;						\
	}								\
}

/* function declarations */
#ifdef AC_PROTO
size_t	strnlen(const char *s, size_t n);
#else
size_t	strnlen();
#endif

/*man---------------------------------------------------------------------------
NAME
     cvtss - convert string

SYNOPSIS
     int cvtss(t, s, m, n)
     char *t;
     const char *s;
     int m;
     int n;

DESCRIPTION
     The cvtss function takes the nul-terminated source string pointed
     to by s, performs the conversions indicated by m, and places the
     result in the target string pointed to by t.  n is the size of
     the target string.  At most n characters are placed in t.

     Values for m are constructed by bitwise OR-ing flags from the
     following list.  These macros are defined in basstr.h.

     CVT_TP               Trim the parity bit.
     CVT_XSP              Discard spaces and tabs.
     CVT_XCTL             Discard control characters.
     CVT_XLEADSP          Discard leading spaces and tabs.
     CVT_1SP              Reduce spaces and tabs to one space.
     CVT_UPPER            Convert lowercase to uppercase.
     CVT_BTOP             Convert [ to ( and ] to ).
     CVT_XTRAILSP         Discard trailing spaces and tabs.
     CVT_QUOTES           Do not alter characters inside single or
                          double quotes except for parity bit
                          trimming.

     cvtss will fail if one or more of the following is true:

     [EINVAL]       t or s is the NULL pointer.
     [EINVAL]       n is less than 0.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

NOTES
     cvtss is adapted directly from the BASIC function CVT$$.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int cvtss(char *t, const char *s, int m, int n)
#else
int cvtss(t, s, m, n)
char *t;
const char *s;
int m;
int n;
#endif
{
	char *	ts	= NULL;		/* temporary source string */
	char *	tt	= NULL;		/* temporary target string */
	char huge *ttend= NULL;		/* first char past end of tt */
	int	tn	= 0;		/* size of ts and tt */
	int	quotes	= 0;		/* preserve quoted characters flag */
	int	bit	= 0;		/* bit number */
	char huge *ps	= NULL;		/* pointer into ts */
	char huge *pt	= NULL;		/* pointer into tt */
	int	flag	= 0;		/* gp flag */

	/* validate arguments */
	if (t == NULL || s == NULL || n < 0) {
		errno = EINVAL;
		return -1;
	}
	if (n < 1) {
		return 0;
	}

	/* find size for ts */
	tn = strlen(s) + 1;
	tn = max(tn, n + 1);

	/* create temporary strings */
	ts = (char *)calloc((size_t)tn, sizeof(*ts));
	if (ts == NULL) {
		errno = ENOMEM;
		return -1;
	}
	tt = (char *)calloc((size_t)tn, sizeof(*tt));
	if (tt == NULL) {
		free(ts);
		errno = ENOMEM;
		return -1;
	}
	ttend = (char huge *)tt + tn;

	/* initialize ts with s */
	strcpy(ts, s);
	ts[tn - 1] = NUL;

	/* perform conversions */
	quotes = m & CVT_QUOTES;	/* set preserve quoted chars flag */
	for (bit = 0; m != 0; ++bit, m >>= 1) {
		if (!(m & 1)) {
			continue;
		}
		switch (bit) {
		case 0:	/* trim the parity bit */
			for (ps = ts, pt = tt; pt < ttend; ++ps) {
				QUOTES;
				*pt++ = *ps & 0x7F;
				if (*ps == NUL) break;
			}
			break;	/* case 0: */
		case 1:	/* discard all spaces and tabs */
			for (ps = ts, pt = tt; pt < ttend; ++ps) {
				QUOTES;
				if (!(*ps == ' ' || *ps == '\t')) {
					*pt++ = *ps;
				}
				if (*ps == NUL) break;
			}
			break;	/* case 1: */
		case 2:	/* discard all control characters */
			for (ps = ts, pt = tt; pt < ttend; ++ps) {
				QUOTES;
				if (*ps == '\t') {
					*pt++ = ' ';
				} else if (!iscntrl(*ps)) {
					*pt++ = *ps;
				}
				if (*ps == NUL) break;
			}
			/* nul terminate if shorter than size */
			if (pt < ttend) {
				*pt++ = NUL;
			}
			break;	/* case 2: */
		case 3:	/* discard leading spaces and tabs */
			for (ps = ts; *ps == ' ' || *ps == '\t'; ++ps) {
			}
			strncpy(tt, (char *)ps, n);
			break;	/* case 3: */
		case 4:	/* reduce spaces and tabs to one space */
			flag = 0;
			for (ps = ts, pt = tt; pt < ttend; ++ps) {
				QUOTES;
				if (flag) {
					if (*ps != ' ' && *ps != '\t') {
						*pt++ = *ps;
						flag = 0;
					}
				} else {
					if (*ps == ' ' || *ps == '\t') {
						*pt++ = ' ';
						flag = 1;
					} else {
						*pt++ = *ps;
					}
				}
				if (*ps == NUL) break;
			}
			break;	/* case 4: */
		case 5:	/* convert lowercase to uppercase */
			for (ps = ts, pt = tt; pt < ttend; ++ps) {
				QUOTES;
				*pt++ = toupper(*ps);
				if (*ps == NUL) break;
			}
			break;	/* case 5: */
		case 6:	/* convert [ to ( and ] to ) */
			for (ps = ts, pt = tt; pt < ttend; ++ps) {
				QUOTES;
				if (*ps == '[') *pt = '(';
				else if (*ps == ']') *pt = ')';
				else *pt = *ps;
				++pt;
				if (*ps == NUL) break;
			}
			break;	/* case 6: */
		case 7:	/* discard trailing spaces and tabs */
			for (ps = (char huge *)ts + strnlen(ts, tn) - 1; ps >= (char huge *)ts; --ps) {
				if (*ps != ' ' && *ps != '\t') break;
			}
			ts[min(tn - 1, ps + 1 - (char huge *)ts)] = NUL;
			strncpy(tt, ts, tn);
			break;	/* case 7: */
		case 8:	/* do not alter characters inside quotes */
			/* see QUOTES macro */
			continue;
			break;	/* case 8: */
		default:
			free(tt);
			free(ts);
			errno = EINVAL;
			return -1;
			break;
		}
		strncpy(ts, tt, tn);
	}

	/* load target string */
	strncpy(t, ts, n);

	/* free temporary strings */
	free(tt);
	free(ts);

	return 0;
}

/* strnlen:  return length of string of size n */
#ifdef AC_PROTO
size_t strnlen(const char *s, size_t n)
#else
size_t strnlen(s, n)
const char *s;
size_t n;
#endif
{
	size_t	i;

	for (i = 0; s[i] != NUL && i < n; ++i);

	return i;
}
