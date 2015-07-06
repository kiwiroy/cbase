/*	Copyright (c) 1991 Citadel	*/
/*	   All Rights Reserved    	*/

/*man---------------------------------------------------------------------------
NAME
     ansi.h - ansi compatibility header

SYNOPSIS
     #include <ansi.h>

DESCRIPTION
     <ansi.h> isolates the definitions and declarations used in making
     source code portable between ANSI C and older versions.  The main
     component of this header is a collection of ANSI compatibility
     macros of the form AC_*.  The definition of each of these macros
     indicates support for a specific feature.  AC_PROTO, for
     instance, is defined if function prototypes are supported.  These
     macros are used in #ifdef constructs such as the following:

          #ifdef AC_PROTO
          int f(int n)
          #else
          int f(n)
          int n;
          #endif

     The new ANSI C keywords (const, signed, void, and volatile) can
     be handled with less effort; if the keyword is not supported,
     that keyword is simply defined in <ansi.h>, effectively removing
     it from the source code.

NOTES
     To install <ansi.h>, first set the AC_* and keyword macros to be
     consistent with the compiler being used.  Next examine the lines
     marked "NON-PORTABLE" to see if any changes are necessary in
     those places.  If a completely ANSI compatible compiler is being
     used, no changes to <ansi.h> should be required.  Once the header
     has been set up correctly, copy it to the include directory
     (e.g., /usr/include for UNIX).

------------------------------------------------------------------------------*/
#ifndef H_ANSI		/* prevent multiple includes */
#define H_ANSI

/* #ident	"@(#)ansi.h	1.1 - 91/08/21" */

/* ANSI C compatibility macros */
#define AC_STRCAT		/* enable string catenation */
#define AC_TOKPST		/* enable token pasting */
#define AC_LDOUBLE		/* enable long double data type */
#define AC_PRINTF		/* enable new printf features (%*) */
#define AC_PROTO		/* enable function prototypes */
#define AC_STDARG		/* enable <stdarg.h> */
#define AC_STDDEF		/* enable <stddef.h> */
#define AC_ESCAPE		/* enable new ansi escape sequences (\a, \v) */
#define AC_STDLIB		/* enable <stdlib.h> */
#define AC_STRING		/* enable <string.h> */
#define AC_VOIDP		/* enable void pointers */
/*#define const			/* disable const keyword */
/*#define signed		/* disable signed keyword */
/*#define void			/* disable void keyword */
/*#define volatile		/* disable volatile keyword */

/* stddef patches */
#ifdef AC_STDDEF
#include <stddef.h>
#else
#include <sys/types.h>		/* size_t typedef -- NON-PORTABLE */
#endif
#ifndef offsetof
#define offsetof(struct_t, member)	((size_t)(char *)&((struct_t *)0)->member)
#endif

/* stdio patches */
#include <stdio.h>
#include <limits.h>
#ifndef FOPEN_MAX
#define FOPEN_MAX	OPEN_MAX	/* NON-PORTABLE */
#endif
#ifndef FILENAME_MAX
#define FILENAME_MAX	NAME_MAX	/* NON-PORTABLE */
#endif

/* stdlib.h patches */
#ifndef AC_STDLIB
#define EXIT_SUCCESS	(0)
#define EXIT_FAILURE	(1)
typedef struct {
	int quot;
	int rem;
} div_t;
typedef struct {
	long int quot;
	long int rem;
} ldiv_t;
#ifdef AC_PROTO
double	atof(const char *);
int	atoi(const char *);
long int atol(const char *);
double	strtod(const char *, char **);
long int strtol(const char *, char **, int);
unsigned long int strtoul(const char *, char **, int);
int	rand(void);
void	srand(unsigned int);
void *	calloc(size_t, size_t);
void *	malloc(size_t);
void *	realloc(void *, size_t);
void	free(void *);
void	abort(void);
int	atexit(void (*)(void));
void	exit(int);
char *	getenv(const char *);
int	system(const char *);
void *	bsearch(const void *, const void *, size_t, size_t, int (*)(const void *, const void *));
void	qsort(void *, size_t, size_t, int (*)(const void *, const void *));
int	abs(int);
div_t	div(int, int);
long int labs(long int);
ldiv_t	ldiv(long int, long int);
int	mblen(const char *, size_t);
/*int	mbtowc(wchar_t *, const char *, size_t);
int	wctomb(char *, wchar_t);
size_t	mbstowcs(wchar_t *, const char *, size_t);
size_t	wcstombs(char *, wchar_t const *, size_t);*/
#else
double	atof();
int	atoi();
long int atol();
double	strtod();
long int strtol();
unsigned long int strtoul();
int	rand();
void	srand();
void *	calloc();
void *	malloc();
void *	realloc();
void	free();
void	abort();
int	atexit();
void	exit();
char *	getenv();
int	system();
void *	bsearch();
void	qsort();
int	abs();
div_t	div();
long int labs();
ldiv_t	ldiv();
int	mblen();
int	mbtowc();
int	wctomb();
size_t	mbstowcs();
size_t	wcstombs();
#endif
#endif

/* string.h patches */
#ifndef AC_STRING
#ifdef AC_PROTO
void *	memcpy(void *, const void *, size_t);
void *	memmove(void *, const void *, size_t);
char *	strcpy(char *, const char *);
char *	strncpy(char *, const char *, size_t);
char *	strcat(char *, const char *);
char *	strncat(char *, const char *, size_t);
int	memcmp(const void *, const void *, size_t);
int	strcmp(const char *, const char *);
int	strcoll(const char *, const char *);
int	strncmp(const char *, const char *, size_t);
size_t	strxfrm(char *, const char *, size_t);
void *	memchr(const void *, int, size_t);
char *	strchr(const char *, int);
size_t	strcspn(const char *, const char *);
char *	strpbrk(const char *, const char *);
char *	strrchr(const char *, int);
size_t	strspn(const char *, const char *);
char *	strstr(const char *, const char *);
char *	strtok(char *, const char *);
void *	memset(void *, int, size_t);
char *	strerror(int);
size_t	strlen(const char *);
#else
void *	memcpy();
void *	memmove();
char *	strcpy();
char *	strncpy();
char *	strcat();
char *	strncat();
int	memcmp();
int	strcmp();
int	strcoll();
int	strncmp();
size_t	strxfrm();
void *	memchr();
char *	strchr();
size_t	strcspn();
char *	strpbrk();
char *	strrchr();
size_t	strspn();
char *	strstr();
char *	strtok();
void *	memset();
char *	strerror();
size_t	strlen();
#endif
#endif

#endif	/* #ifndef H_ANSI */
