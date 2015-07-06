/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/*man---------------------------------------------------------------------------
NAME
     basstr - BASIC string library

SYNOPSIS
     #include "basstr.h"

DESCRIPTION
     C library of string functions adapted from BASIC.

SEE ALSO
     cvtss.

------------------------------------------------------------------------------*/
#ifndef BASSTR_H	/* prevent multiple includes */
#define BASSTR_H

/* #ident	"@(#)basstr.h	1.5 - 91/09/23" */

#include <ansi.h>

/* macros */
#define NUL	('\0')	/* nul character */

/* function declarations */
#ifdef AC_PROTO
int cvtss(char *t, const char *s, int m, int n);
#else
int cvtss();
#endif

/* cvtss m macros */
#define CVT_TP		(0x0001)	/* trim the parity bit */
#define CVT_XSP		(0x0002)	/* discard spaces and tabs */
#define CVT_XCTL	(0x0004)	/* discard control characters */
#define CVT_XLEADSP	(0x0008)	/* discard leading spaces and tabs */
#define CVT_1SP		(0x0010)	/* reduce spaces, tabs to one space */
#define CVT_UPPER	(0x0020)	/* convert lowercase to uppercase */
#define CVT_BTOP	(0x0040)	/* convert [ to ( and ] to ) */
#define CVT_XTRAILSP	(0x0080)	/* discard trailing spaces and tabs */
#define CVT_QUOTES	(0x0100)	/* do not alter chars inside single or
					 double quotes except for parity bit
					 trimming */

#endif	/* #ifndef BASSTR_H */
