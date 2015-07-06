/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)buops.c	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#include <errno.h>
#ifdef AC_STRING
#include <string.h>
#endif

/* local headers */
#include "blkio_.h"

/* check host definition */
#ifndef OPSYS
/*#error OPSYS macro not defined.*/
#endif
#if OPSYS < OS_AMIGADOS || OPSYS > OS_VMS
/*#error Invalid OPSYS macro definition.*/
#endif

/* system headers */
#if OPSYS == OS_AMIGADOS	/* Amiga DOS =================================*/

#elif OPSYS == OS_DOS		/* DOS =======================================*/
#if CCOM == CC_BC		/* Borland C++ -------------------------------*/
#include <fcntl.h>		/* open() macro definitions */
#include <io.h>			/* system call decls, lseek() macro defs */
#include <share.h>		/* file sharing macros */
#include <sys\types.h>
#include <sys\stat.h>		/* file permission macros */
#define MODE	(S_IREAD | S_IWRITE)
				/* file permissions for new files */
#elif CCOM == CC_MSC || CCOM == CC_MSQC	/* Microsoft C or Quick C ------------*/
#include <fcntl.h>		/* open() macro definitions */
/*#include <io.h>		/* system call decls, lseek() macro defs */
/* NOTE:  <io.h> not used for MSC because const qualifier missing from
   sopen and write decls */
#include <share.h>		/* file sharing macros */
#include <sys\types.h>
#include <sys\stat.h>		/* file permission macros */
#ifdef AC_PROTO
int	close(int fd);		/* system call declarations */
long	lseek(int fd, long offset, int whence);
int	sopen(const char *path, int oflag, ...);
int	read(int fd, char *buf, unsigned n);
int	write(int fd, const char *buf, unsigned n);
#else
int	close();
long	lseek();
int	sopen();
int	read();
int	write();
#endif	/* #ifdef AC_PROTO */
#define MODE	(S_IREAD | S_IWRITE)
				/* file permissions for new files */
#endif	/* #if CCOM == CC_BC */
#elif OPSYS == OS_MAC		/* Macintosh =================================*/

#elif OPSYS == OS_UNIX		/* UNIX ======================================*/
#include <fcntl.h>		/* open() macro definitions */
#include <unistd.h>		/* lseek() macro definitions */
#include <sys/types.h>
#include <sys/stat.h>		/* file permission macros */
#ifdef AC_PROTO
int	close(int fd);		/* system call declarations */
long	lseek(int fd, long offset, int whence);
int	open(const char *path, int flags, ...);
int	read(int fd, char *buf, unsigned n);
int	write(int fd, const char *buf, unsigned n);
#else
int	close();
long	lseek();
int	open();
int	read();
int	write();
#endif	/* #ifdef AC_PROTO */
#define MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)
				/* file permissions for new files */
#elif OPSYS == OS_VMS		/* VMS =======================================*/

#endif	/* #if OPSYS == OS_AMIGADOS */

/*man---------------------------------------------------------------------------
NAME
     b_uclose - unbuffered close block file

SYNOPSIS
     #include "blkio_.h"

     int b_uclose(bp)
     BLKFILE *bp;

DESCRIPTION
     The b_uclose function closes the file associated with the BLKFILE
     pointer bp.

     b_uclose will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [BENOPEN]      bp is not open.

SEE ALSO
     b_uopen.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int b_uclose(BLKFILE *bp)
#else
int b_uclose(bp)
BLKFILE *bp;
#endif
{
#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp)) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOOPEN)) {
		BEPRINT;
		errno = BENOPEN;
		return -1;
	}
#endif
	/* close file */
#if OPSYS == OS_AMIGADOS

#elif OPSYS == OS_DOS || OPSYS == OS_UNIX
	if (close(bp->fd.i) == -1) {
		BEPRINT;
		return -1;
	}
#elif OPSYS == OS_MAC

#elif OPSYS == OS_VMS

#endif

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_uendblk - unbuffered find end block

SYNOPSIS
     #include "blkio_.h"

     int b_uendblk(bp, endblkp)
     BLKFILE *bp;
     bpos_t *endblkp;

DESCRIPTION
     The b_uendblk function finds the block number of the first block
     past the end of file of the file associated with BLKFILE pointer
     bp.  This value is returned in the storage location pointed to by
     endblkp.  Blocks in buffer storage and not yet written to the
     file are not counted, so this function should normally be used
     when the file is first opened or preceded by a call to bsync.

     If the file does not end on a block boundary, the result is the
     same as if the partial block (or header) at the end of the file
     did not exist.

     b_uendblk will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [BENOPEN]      bp is not open.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int b_uendblk(BLKFILE *bp, bpos_t *endblkp)
#else
int b_uendblk(bp, endblkp)
BLKFILE *bp;
bpos_t *endblkp;
#endif
{
#if OPSYS == OS_AMIGADOS

#elif OPSYS == OS_DOS || OPSYS == OS_UNIX
	long	pos	= 0;
#elif OPSYS == OS_MAC

#elif OPSYS == OS_VMS

#endif

#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp)) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOOPEN)) {
		BEPRINT;
		errno = BENOPEN;
		return -1;
	}
#endif

#if OPSYS == OS_AMIGADOS

#elif OPSYS == OS_DOS || OPSYS == OS_UNIX
	/* find position of end of file */
	pos = lseek(bp->fd.i, (long)0, SEEK_END);
	if (pos == -1) {
		BEPRINT;
		return -1;
	}

	/* check if empty file (or incomplete header) */
	if (pos < bp->hdrsize) {
		*endblkp = 0;
		return 0;
	}

	/* find length past end of header */
	pos -= bp->hdrsize;

	/* set return argument */
	*endblkp = (pos / bp->blksize) + 1;
#elif OPSYS == OS_MAC

#elif OPSYS == OS_VMS

#endif

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_ugetf - unbuffered get field from block file

SYNOPSIS
     #include "blkio_.h"

     int b_ugetf(bp, bn, offset, buf, bufsize)
     BLKFILE *bp;
     bpos_t bn;
     size_t offset;
     void *buf;
     size_t bufsize;

DESCRIPTION
     The b_ugetf function reads bufsize characters from block number
     bn of the block file associated with BLKFILE pointer bp into the
     buffer pointed to by buf.  A value of 0 for bn indicates the file
     header.  The read starts offset characters from the beginning of
     the block.  The sum of the offset and the bufsize must not exceed
     the header size if bn is equal to zero, or the block size if bn
     is not equal to zero.

     b_ugetf will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       buf is NULL.
     [EINVAL]       bufsize is less than 1.
     [BEBOUND]      offset + bufsize extends across a block
                    boundary.
     [BEEOF]        Block bn is past the end of file.
     [BEEOF]        End of file encountered within block bn.
     [BENOPEN]      bp is not open.

SEE ALSO
     b_uputf.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int b_ugetf(BLKFILE *bp, bpos_t bn, size_t offset, void *buf, size_t bufsize)
#else
int b_ugetf(bp, bn, offset, buf, bufsize)
BLKFILE *bp;
bpos_t bn;
size_t offset;
void *buf;
size_t bufsize;
#endif
{
#if OPSYS == OS_AMIGADOS


#elif OPSYS == OS_DOS || OPSYS == OS_UNIX
	int	nr	= 0;
	long	pos	= 0;
#elif OPSYS == OS_MAC

#elif OPSYS == OS_VMS

#endif

#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp) || buf == NULL || bufsize < 1) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOOPEN)) {
		BEPRINT;
		errno = BENOPEN;
		return -1;
	}

	/* check if block boundary is crossed */
	if (bn == 0) {
		if ((offset + bufsize) > bp->hdrsize) {
			BEPRINT;
			errno = BEBOUND;
			return -1;
		}
	} else {
		if ((offset + bufsize) > bp->blksize) {
			BEPRINT;
			errno = BEBOUND;
			return -1;
		}
	}
#endif

#if OPSYS == OS_AMIGADOS

#elif OPSYS == OS_DOS || OPSYS == OS_UNIX
	/* read from file into buffer */
	if (bn == 0) {			/* header */
		pos = 0;
	} else {			/* block */
		pos = bp->hdrsize + (bn - 1) * bp->blksize;
	}
	pos += offset;
	if (lseek(bp->fd.i, pos, SEEK_SET) == -1) {
		BEPRINT;
		return -1;
	}
	nr = read(bp->fd.i, buf, (unsigned)bufsize);
	if (nr == -1) {
		BEPRINT;
		return -1;
	}
	if (nr != bufsize) {
		BEPRINT;
		errno = BEEOF;
		return -1;
	}
#elif OPSYS == OS_MAC

#elif OPSYS == OS_VMS

#endif

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_uopen - unbuffered open block file

SYNOPSIS
     #include "blkio_.h"

     int b_uopen(bp, filename, type)
     BLKFILE *bp;
     const char *filename;
     const char *type;

DESCRIPTION
     The b_uopen function opens the physical file associated with the
     BLKFILE pointer bp.

     b_uopen will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       filename or type is the NULL pointer.

SEE ALSO
     b_uclose.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int b_uopen(BLKFILE *bp, const char *filename, const char *type)
#else
int b_uopen(bp, filename, type)
BLKFILE *bp;
const char *filename;
const char *type;
#endif
{
#if OPSYS == OS_AMIGADOS

#elif OPSYS == OS_DOS || OPSYS == OS_UNIX
	int	fd	= 0;
	int	oflag	= 0;
#elif OPSYS == OS_MAC

#elif OPSYS == OS_VMS

#endif

#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp) || filename == NULL || type == NULL) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}
#endif
	/* open file */
#if OPSYS == OS_AMIGADOS

#elif OPSYS == OS_DOS
	oflag = O_BINARY;
	if (strcmp(type, BF_READ) == 0) {
		oflag |= O_RDONLY;
	} else if (strcmp(type, BF_RDWR) == 0) {
		oflag |= O_RDWR;
	} else if (strcmp(type, BF_CREATE) == 0) {
		oflag |= O_RDWR | O_CREAT | O_EXCL;
	} else if (strcmp(type, BF_CRTR) == 0) {
		oflag |= O_RDWR | O_CREAT | O_TRUNC;
	} else {
		errno = EINVAL;
		return -1;
	}
	fd = sopen(filename, oflag, SH_DENYNO, MODE);
	if (fd == -1) {
#ifdef DEBUG
		if (errno != EACCES && errno != EEXIST && errno != ENOENT) BEPRINT;
#endif
		return -1;
	}
	bp->fd.i = fd;
#elif OPSYS == OS_UNIX
	oflag = 0;
	if (strcmp(type, BF_READ) == 0) {
		oflag |= O_RDONLY;
	} else if (strcmp(type, BF_RDWR) == 0) {
		oflag |= O_RDWR;
	} else if (strcmp(type, BF_CREATE) == 0) {
		oflag |= O_RDWR | O_CREAT | O_EXCL;
	} else if (strcmp(type, BF_CRTR) == 0) {
		oflag |= O_RDWR | O_CREAT | O_TRUNC;
	} else {
		errno = EINVAL;
		return -1;
	}
	fd = open(filename, oflag, MODE);
	if (fd == -1) {
#ifdef DEBUG
		if (errno != EACCES && errno != EEXIST && errno != ENOENT) BEPRINT;
#endif
		return -1;
	}
	bp->fd.i = fd;
#elif OPSYS == OS_VMS

#endif

	return 0;
}

/*man---------------------------------------------------------------------------
NAME
     b_uputf - unbuffered put field to block file

SYNOPSIS
     #include "blkio_.h"

     int b_uputf(bp, bn, offset, buf, bufsize)
     BLKFILE *bp;
     bpos_t bn;
     size_t offset;
     const void *buf;
     size_t bufsize;

DESCRIPTION
     The b_uputf function writes bufsize characters from the buffer
     pointed to by buf to block number bn of the block file associated
     with BLKFILE pointer bp.  A value of zero for bn indicates the
     file header.  The write starts offset characters from the
     beginning of the block.  The sum of offset bufsize must not
     exceed the header size if bn is equal to zero, or the block size
     if bn is not equal to zero.

     b_uputf will fail if one or more of the following is true:

     [EINVAL]       bp is not a valid BLKFILE pointer.
     [EINVAL]       buf is the NULL pointer.
     [EINVAL]       bufsize is less than 1.
     [BEBOUND]      offset + bufsize extends beyond the block
                    boundary.
     [BEEOF]        Partial block being written and block bn
                    is past the end of file.
     [BEEOF]        Complete block being written and block bn
                    is more than 1 past the end of file.
     [BENOPEN]      bp is not open for writing.

SEE ALSO
     b_ugetf.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned, and errno set to indicate the error.

------------------------------------------------------------------------------*/
#ifdef AC_PROTO
int b_uputf(BLKFILE *bp, bpos_t bn, size_t offset, const void *buf, size_t bufsize)
#else
int b_uputf(bp, bn, offset, buf, bufsize)
BLKFILE *bp;
bpos_t bn;
size_t offset;
const void *buf;
size_t bufsize;
#endif
{
#if OPSYS == OS_AMIGADOS

#elif OPSYS == OS_DOS || OPSYS == OS_UNIX
	int	nw	= 0;
	long	pos	= 0;
#elif OPSYS == OS_MAC

#elif OPSYS == OS_VMS

#endif

#ifdef DEBUG
	/* validate arguments */
	if (!b_valid(bp) || buf == NULL || bufsize < 1) {
		BEPRINT;
		errno = EINVAL;
		return -1;
	}

	/* check if not open */
	if (!(bp->flags & BIOWRITE)) {
		BEPRINT;
		errno = BENOPEN;
		return -1;
	}

	/* check if block boundary is crossed */
	if (bn == 0) {
		if ((offset + bufsize) > bp->hdrsize) {
			BEPRINT;
			errno = BEBOUND;
			return -1;
		}
	} else {
		if ((offset + bufsize) > bp->blksize) {
			BEPRINT;
			errno = BEBOUND;
			return -1;
		}
	}
#endif

#if OPSYS == OS_AMIGADOS

#elif OPSYS == OS_DOS || OPSYS == OS_UNIX
	/* write buffer to file */
	if (bn == 0) {			/* header */
		pos = 0;
	} else {			/* block */
		pos = bp->hdrsize + (bn - 1) * bp->blksize;
	}
	pos += offset;
	if (lseek(bp->fd.i, pos, SEEK_SET) == -1) {
		BEPRINT;
		return -1;
	}
	nw = write(bp->fd.i, (char *)buf, (unsigned)bufsize);
	if (nw == -1) {
		BEPRINT;
		return -1;
	}
	if (nw != bufsize) {
		BEPRINT;
		/* call write again to set errno (EFBIG or ENOSPC) */
		nw = write(bp->fd.i, (char *)buf, (unsigned)1);
		if (nw != -1) {
			BEPRINT;
			errno = BEPANIC;
		}
		return -1;
	}
#elif OPSYS == OS_MAC

#elif OPSYS == OS_VMS

#endif

	return 0;
}
