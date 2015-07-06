/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)ansi.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif
#include <stdio.h>

/* function declarations */
#ifdef AC_PROTO
int link(const char *path1, const char *path2);
int unlink(const char *path);
#else
int link();
int unlink();
#endif

/* global data declarations */
extern char *sys_errlist[];
extern int sys_nerr;

/*man---------------------------------------------------------------------------
NAME
    ansi - ANSI functions required by cbase

DESCRIPTION
    This file contains ANSI functions required by cbase which may not
    be available with older compilers.  It is likely to require
    editing.

------------------------------------------------------------------------------*/
#ifndef AC_STRING
/* memmove:  memory move */
#ifdef AC_PROTO
void *memmove(void *t, const void *s, size_t n)
#else
void *memmove(t, s, n)
void *t;
const void *s;
size_t n;
#endif
{
	int i = 0;

	if (t < s) {
		for (i = 0; i < n; ++i) {
			*((char *)t + i) = *((char *)s + i);
		}
	} else {
		for (i = n - 1; i >= 0; --i) {
			*((char *)t + i) = *((char *)s + i);
		}
	}

	return t;
}
#endif

#ifdef AC_PROTO
void perror(const char *s)
#else
void perror(s)
const char *s;
#endif
{
	if (s != NULL) {
		fputs(s, stderr);
		fputs(": ", stderr);
	}
	if (errno >= 0 && errno < sys_nerr) {
		fprintf(stderr, "%s\n", sys_errlist[errno]);
	} else {
		fprintf(stderr, "Error number %d\n", errno);
	}

	return;
}

#ifdef AC_PROTO
int remove(const char *filename)
#else
int remove(filename)
const char *filename;
#endif
{
	if (unlink(filename) == -1) {
		return -1;
	}

	return 0;
}

#ifdef AC_PROTO
int rename(const char *file1, const char *file2)
#else
int rename(file1, file2)
const char *file1;
const char *file2;
#endif
{
	if (link(file1, file2) == -1) {
		return -1;
	}
	if (unlink(file1) == -1) {
		return -1;
	}

	return 0;
}
