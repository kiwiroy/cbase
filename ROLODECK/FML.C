/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)fml.c	1.5 - 91/09/23" */

#include <ansi.h>

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

/* constants */
#define NUL	('\0')	/* nul character */

/* macros */
#define min(a,b)	((a) < (b) ? (a) : (b))

/*man---------------------------------------------------------------------------
NAME
     fmltolfm - convert name from fml to lfm format

SYNOPSIS
     int fmltolfm(t, s, n)
     char *t;
     const char *s;
     size_t n;

DESCRIPTION
     The fmltolfm function converts a name from the format "first
     middle last" to "last first middle".  s points to the
     nul-terminated source string in fml format.  t points to the
     target string to receive the converted name.  t and s may point
     to the same string.  There may be more than one middle name, or
     the middle name may be absent.  All leading and trailing spaces
     must be removed from the source string before calling fmltolfm.
     At most n characters are copied.  The target will be padded with
     nuls if the name has fewer than n characters.

     fmltolfm will fail if one or more of the following is true:

     [EINVAL]       t or s is the NULL pointer.

SEE ALSO
     lfmtofml.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int fmltolfm(char *t, const char *s, size_t n)
#else
int fmltolfm(t, s, n)
char *t;
const char *s;
size_t n;
#endif
{
	size_t	len	= 0;		/* string length */
	char *	p	= NULL;
	char *	ts	= NULL;		/* temporary string */

	/* validate arguments */
	if (t == NULL || s == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* get length of name */
	len = strlen(s);

	/* find beginning of last name */
	p = strrchr(s, ' ');
	if (p == NULL) {
		strncpy(t, s, n);
		return 0;
	}

	/* create temporary string */
	ts = (char *)calloc(n + 1, (size_t)1);
	if (ts == NULL) {
		errno = ENOMEM;
		return -1;
	}

	/* perform conversion */
	strncpy(ts, p + 1, n);			/* copy last name */
	ts[n] = NUL;
	strncat(ts, " ", n - strlen(ts));	/* add space after last name */
	ts[n] = NUL;
	strncat(ts, s, n - strlen(ts));		/* copy beginning of name */
	if (len < n) {		/* remove space that marked last name */
		ts[len] = NUL;
	}
	strncpy(t, ts, n);			/* copy converted string to t */

	/* free temporary string */
	free(ts);

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     lfmtofml - convert name from lfm to fml format

SYNOPSIS
     int lfmtofml(t, s, n)
     char *t;
     const char *s;
     size_t n;

DESCRIPTION
     The lfmtofml function converts a name from the format "last
     first middle" to "first middle last".  s points to the
     nul-terminated source string in lfm format.  t points to the
     target string to receive the converted name.  t and s may point
     to the same string.  There may be more than one middle name, or
     the middle name may be absent.  All leading and trailing spaces
     must be removed from the source string before calling fmltolfm.
     At most n characters are copied.  The target will be padded with
     nuls if the name has fewer than n characters.

     lfmtofml will fail if one or more of the following is true:

     [EINVAL]       t or s is the NULL pointer.

SEE ALSO
     fmltolfm.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lfmtofml(char *t, const char *s, size_t n)
#else
int lfmtofml(t, s, n)
char *t;
const char *s;
size_t n;
#endif
{
	size_t	len	= 0;		/* string length */
	char *	p	= NULL;
	char *	ts	= NULL;		/* temporary string */

	/* validate arguments */
	if (t == NULL || s == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* get length of name */
	len = strlen(s);

	/* find end of last name */
	p = strchr(s, ' ');
	if (p == NULL) {
		strncpy(t, s, n);
		return 0;
	}

	/* create temporary string */
	ts = (char *)calloc(n + 1, (size_t)1);
	if (ts == NULL) {
		errno = ENOMEM;
		return -1;
	}

	/* perform conversion */
	strncpy(ts, p + 1, n);			/* copy beginning of name */
	ts[n] = NUL;
	strncat(ts, " ", n - strlen(ts));	/* add space before last name */
	ts[n] = NUL;
	strncat(ts, s, n - strlen(ts));		/* copy last name */
	if (len < n) {		/* remove space that marked first name */
		ts[len] = NUL;
	}
	strncpy(t, ts, n);			/* copy converted string to t */

	/* free temporary string */
	free(ts);

	return 0;
}
