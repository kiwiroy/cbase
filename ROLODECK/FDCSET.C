/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)fdcset.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STRING
#include <string.h>
#endif

/* system headers */
#include <dos.h>

extern int _doserrno;

/*man---------------------------------------------------------------------------
NAME
     fdcset - set file descriptor count

SYNOPSIS
     int fdcset(fdc)
     int fdc;

DESCRIPTION
     The fdcset function sets the file descriptor count of the calling
     process to fdc.  fdc can be no more than the number of entries in
     the system file table set by FILES= in config.sys.

     This function is for DOS only, and requires version 3.3 or higher.

     fdcset will fail if one or more of the following is true:

     [EINVAL]       fdc is less than 1.
     [ENOMEM]       Not enough memory for expanded file descriptor
                    table.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int fdcset(int fdc)
#else
int fdcset(fdc)
#endif
{
	union REGS	regs;		/* registers */

	/* validate arguments */
	if (fdc < 1) {
		errno = EINVAL;
		return -1;
	}

	/* call DOS function 67h to set fildes count */
	memset(&regs, 0, sizeof(regs));
	regs.h.ah = 0x67;
	regs.x.bx = fdc;
	(void)intdos(&regs, &regs);
	if (regs.x.cflag) {
		if (_doserrno == ENOMEM) {
			errno = ENOMEM;
		}
		return -1;
	}

	return 0;
}
