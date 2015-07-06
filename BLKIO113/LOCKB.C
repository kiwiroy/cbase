/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)lockb.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STDDEF
#include <stddef.h>
#endif

/* local headers */
#include "blkio_.h"

/* system headers */
#if OPSYS == OS_AMIGADOS	/* Amiga DOS =================================*/

#elif OPSYS == OS_MAC		/* Macintosh =================================*/

#elif OPSYS == OS_DOS		/* DOS =======================================*/
#include <limits.h>		/* LONG_MAX definition */
#if CCOM == CC_BC		/* Borland C++ -------------------------------*/
#include <io.h>
#define LK_UNLCK	(0)	/* unlock */
#define LK_LOCK		(1)	/* lock */
#define LK_NBLCK	(2)	/* non-blocking lock */
#define LK_RLCK		(3)	/* read lock */
#define LK_NBRLCK	(4)	/* non-blocking read lock */
#elif CCOM == CC_MSC || CCOM == CC_MSQC	/* Microsoft C or Quick C ------------*/
#include <sys\locking.h>
#include <io.h>
#endif
#elif OPSYS == OS_UNIX		/* UNIX ======================================*/
#include <fcntl.h>
#ifdef AC_PROTO
int fcntl(int fd, int cmd, ...);
#else
int fcntl();
#endif
#elif OPSYS == OS_VMS		/* VMS =======================================*/

#endif

/*man---------------------------------------------------------------------------
NAME
     lockb - block file record locking

SYNOPSIS
     #include <blkio.h>

     int lockb(bp, ltype, start, len)
     BLKFILE *bp;
     int ltype;
     bpos_t start;
     bpos_t len;

DESCRIPTION
     The lockb function will allow segments of a block file to be
     locked.  bp is the BLKFILE pointer for the file to be locked.

     ltype indicates the target status of the lock.  The lock types
     available are:

       B_UNLCK unlock block file segment
       B_RDLCK lock block file segment for reading
       B_WRLCK lock block file segment for reading and writing
       B_RDLKW lock block file segment for reading (wait)
       B_WRLKW lock block file segment for reading and writing (wait)

     For the lock types which wait, lockb will not return until the
     lock is available.  For the lock types which do not wait, if the
     lock is unavailable because of a lock held by another process  a
     value of -1 is returned and errno set to EAGAIN.

     start is the first block to lock.  len is the number of
     contiguous blocks including and following block start to be
     locked or unlocked.  A lock may be set to extend to the end of
     the file by setting len to zero.

     The buffers are flushed before unlocking.

     lockb will fail if one or more of the following is true:

     [EAGAIN]       ltype is B_RDLCK and the file segment to be locked
                    is already write locked by another process, or
                    ltype is B_WRLCK and the file segment to be locked
                    is already read or write locked by another
                    process.
     [EINVAL]       bp is is not a valid BLKFILE pointer.
     [EINVAL]       ltype is not one of the valid lock types.
     [BENOPEN]      bp is not open.
     [BENOPEN]      ltype is B_RDLCK or B_RDLKW and bp is not opened
                    for reading or ltype is B_WRLCK or B_WRLKW and bp
                    is not open for writing.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int lockb(BLKFILE *bp, int ltype, bpos_t start, bpos_t len)
#else
int lockb(bp, ltype, start, len)
BLKFILE *bp;
int ltype;
bpos_t start;
bpos_t len;
#endif
{
#if OPSYS == AMIGADOS

#elif OPSYS == OS_DOS
	long	length	= 0;	/* length in bytes of segment to lock */
	int	mode	= 0;	/* file locking mode */
	long	offset	= 0;	/* offset from start of file */
#elif OPSYS == OS_MAC

#elif OPSYS == OS_UNIX
	int	cmd	= 0;	/* lock command */
	struct flock lck;	/* lock structure */
#elif OPSYS == OS_VMS

#endif
	/* validate arguments */
	if (!b_valid(bp)) {
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOOPEN)) {
		errno = BENOPEN;
		return -1;
	}

	/* set lock flags */
	switch (ltype) {
	case B_RDLCK:
		if (!(bp->flags & BIOREAD)) {
			errno = BENOPEN;
			return -1;
		}
#if OPSYS == AMIGADOS

#elif OPSYS == OS_DOS
		mode = LK_NBRLCK;
#elif OPSYS == OS_MAC

#elif OPSYS == OS_UNIX
		cmd = F_SETLK;
		lck.l_type = F_RDLCK;
#elif OPSYS == OS_VMS

#endif
		break;
	case B_RDLKW:
		if (!(bp->flags & BIOREAD)) {
			errno = BENOPEN;
			return -1;
		}
#if OPSYS == AMIGADOS

#elif OPSYS == OS_DOS
		mode = LK_RLCK;
#elif OPSYS == OS_MAC

#elif OPSYS == OS_UNIX
		cmd = F_SETLKW;
		lck.l_type = F_RDLCK;
#elif OPSYS == OS_VMS

#endif
		break;
	case B_WRLCK:
		if (!(bp->flags & BIOWRITE)) {
			errno = BENOPEN;
			return -1;
		}
#if OPSYS == AMIGADOS

#elif OPSYS == OS_DOS
		mode = LK_NBLCK;
#elif OPSYS == OS_MAC

#elif OPSYS == OS_UNIX
		cmd = F_SETLK;
		lck.l_type = F_WRLCK;
#elif OPSYS == OS_VMS

#endif
		break;
	case B_WRLKW:
		if (!(bp->flags & BIOWRITE)) {
			errno = BENOPEN;
			return -1;
		}
#if OPSYS == AMIGADOS

#elif OPSYS == OS_DOS
		mode = LK_LOCK;
#elif OPSYS == OS_MAC

#elif OPSYS == OS_UNIX
		cmd = F_SETLKW;
		lck.l_type = F_WRLCK;
#elif OPSYS == OS_VMS

#endif
		break;
	case B_UNLCK:
		/* flush buffers */
		if (bflush(bp) == -1) {
			BEPRINT;
			return -1;
		}
#if OPSYS == AMIGADOS

#elif OPSYS == OS_DOS
		mode = LK_UNLCK;
#elif OPSYS == OS_MAC

#elif OPSYS == OS_UNIX
		cmd = F_SETLK;
		lck.l_type = F_UNLCK;
#elif OPSYS == OS_VMS

#endif
		break;
	default:
		errno = EINVAL;
		return -1;
		break;
	}

	/* lock */
#ifndef SINGLE_USER
#if OPSYS == AMIGADOS

#elif OPSYS == OS_DOS
	if (start == 0) {
		offset = 0;
		if (len == 0) {
			length = LONG_MAX;
		} else {
			length = bp->hdrsize + (len - 1) * bp->blksize;
		}
	} else {
		offset = bp->hdrsize + (start - 1) * bp->blksize;
		if (len == 0) {
			length = LONG_MAX - offset;
		} else {
			length = len * bp->blksize;
		}
	}
#if CCOM == CC_BC
	switch (mode) {
	case LK_UNLCK:	/* unlock */
		if (unlock(bp->fd.i, offset, length) == -1) {
			/* ignore 'segment not locked' error */
			if (errno == EACCES) return 0;
			return -1;
		}
		break;
	default:	/* lock */
		if (lock(bp->fd.i, offset, length) == -1) {
			if (errno == EACCES) errno = EAGAIN;
			if (errno != EAGAIN) BEPRINT;
			return -1;
		}
		break;
	}
#elif CCOM == CC_MSC || CCOM == CC_MSQC
	if (lseek(bp->fd.i, offset, SEEK_SET) == -1) {
		BEPRINT;
		return -1;
	}
	if (locking(bp->fd.i, mode, length) == -1) {
		/* ignore 'segment not locked' error */
		if (mode == LK_UNLCK) {
			if (errno == EACCES) return 0;
		}
		if (errno == EACCES) errno = EAGAIN;
		if (errno != EAGAIN) BEPRINT;
		return -1;
	}
#endif
#elif OPSYS == OS_MAC

#elif OPSYS == OS_UNIX
	if (start == 0) {
		lck.l_whence = 0;
		if (len == 0) {
			lck.l_len = 0;
		} else {
			lck.l_len = bp->hdrsize + (len - 1) * bp->blksize;
		}
	} else {
		lck.l_whence = bp->hdrsize + (start - 1) * bp->blksize;
		if (len == 0) {
			lck.l_len = 0;
		} else {
			lck.l_len = len * bp->blksize;
		}
	}
	lck.l_start = 0;
/*	lck.l_sysid = 0; l_sysid not defined by BSD UNIX */
	lck.l_pid = 0;
	if (fcntl(bp->fd.i, cmd, &lck) == -1) {
		/* new versions of fcntl will use EAGAIN */
		if (errno == EACCES) errno = EAGAIN;
		if (errno != EAGAIN) BEPRINT;
		return -1;
	}
#elif OPSYS == OS_VMS

#endif
#endif	/* #ifndef SINGLE_USER */

	/* if locking, load endblk */
	if (ltype != B_UNLCK) {
		if (b_uendblk(bp, &bp->endblk) == -1) {
			BEPRINT;
			return -1;
		}
	}

	return 0;
}
