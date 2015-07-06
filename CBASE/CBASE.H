/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/*man---------------------------------------------------------------------------
NAME
     cbase - C database library

SYNOPSIS
     #include <cbase.h>

DESCRIPTION
     cbase is a database file managment library for indexed and
     sequential access to records.  Below are listed the predefined
     data types provided with cbase.  See the the cbase Programmer's
     Guide for information on adding new data types.

     t_char         signed character
     t_charv        signed character array
     t_uchar        unsigned character
     t_ucharv       unsigned character array
     t_short        signed short integer
     t_shortv       signed short integer array
     t_ushort       unsigned short integer
     t_ushortv      unsigned short integer array
     t_int          signed integer
     t_intv         signed integer array
     t_uint         unsigned integer
     t_uintv        unsigned integer array
     t_long         long integer
     t_longv        long integer array
     t_ulong        unsigned long integer
     t_ulongv       unsigned long integer array
     t_float        float
     t_floatv       float array
     t_double       double
     t_doublev      double array
     t_ldouble      long double
     t_ldoublev     long double array
     t_pointer      pointer
     t_pointerv     pointer array
     t_string       character string
     t_cistring     case-insensitive string
     t_binary       block of binary data (e.g., graphics)

SEE ALSO
     cbclose, cbcreate, cbdelcur, cbexport, cbgetkcur, cbgetlck,
     cbgetr, cbgetrcur, cbgetrf, cbimport, cbinsert, cbkcursor,
     cbkeyalign, cbkeyfirst, cbkeylast, cbkeynext, cbkeyprev,
     cbkeysrch, cblock, cbmkndx, cbopen, cbputr, cbrcursor,
     cbrecalign, cbreccnt, cbrecfirst, cbreclast, cbrecnext,
     cbrecprev, cbrecsize, cbrmndx, cbsetkcur, cbsetrcur, cbsync.

------------------------------------------------------------------------------*/
#ifndef H_CBASE		/* prevent multiple includes */
#define H_CBASE

/* #ident	"@(#)cbase.h	1.5 - 91/09/23" */

#include <ansi.h>

/* ansi headers */
#ifdef AC_STDDEF
#include <stddef.h>
#endif

/* library headers */
#include <btree.h>
#include <lseq.h>

/* constants */
#define CBOPEN_MAX	BOPEN_MAX	/* max # cbases open at once */
#define CBM		(11)		/* btree order */

/* type definitions */
typedef lspos_t	cbrpos_t;		/* cbase record file position */
typedef btpos_t cbkpos_t;		/* cbase key file position */

typedef struct {			/* field definition */
	size_t offset;			/* offset of field in record */
	size_t len;			/* field length */
	int type;			/* field data type */
	int flags;			/* flags */
	char *filename;			/* index file name */
} cbfield_t;

typedef struct {			/* cbase control structure */
	lseq_t *lsp;			/* lseq file containing records */
	int flags;			/* status flags */
	int fldc;			/* field count */
	cbfield_t *fldv;		/* field definitions */
	btree_t **btpv;			/* btree containing keys */
} cbase_t;

/* cbfield_t bit flags */
#define CB_FFLAGS	  (03)	/* mask for all flags */
#define CB_FKEY		  (01)	/* field is a key */	
#define CB_FUNIQ	  (02)	/* constrain key to be unique */

/* function declarations */
#ifdef AC_PROTO
int		cbclose(cbase_t *cbp);
int		cbcreate(const char *cbname, size_t recsize, int fldc,
			const cbfield_t fldv[]);
int		cbdelcur(cbase_t *cbp);
int		cbexport(cbase_t *cbp, const char *filename);
int		cbgetkcur(cbase_t *cbp, int field, cbkpos_t *cbkposp);
int		cbgetlck(cbase_t *cbp);
int		cbgetr(cbase_t *cbp , void *buf);
int		cbgetrcur(cbase_t *cbp, cbrpos_t *cbrposp);
int		cbgetrf(cbase_t *cbp, int field, void *buf);
int		cbimport(cbase_t *cbp, const char *filename);
int		cbinsert(cbase_t *cbp, const void *buf);
int		cbkeyalign(cbase_t *cbp, int field);
int		cbkeyfirst(cbase_t *cbp, int field);
int		cbkeylast(cbase_t *cbp, int field);
int		cbkeynext(cbase_t *cbp, int field);
int		cbkeyprev(cbase_t *cbp, int field);
int		cbkeysrch(cbase_t *cbp, int field, const void *buf);
int		cblock(cbase_t *cbp, int ltype);
int		cbmkndx(cbase_t *cbp, int field, int flags, char *filename);
cbase_t *	cbopen(const char *cbname, const char *type, int cbfldc,
			const cbfield_t fldv[]);
int		cbputr(cbase_t *cbp , const void *buf);
int		cbrecalign(cbase_t *cbp, int field);
int		cbrecfirst(cbase_t *cbp);
int		cbreclast(cbase_t *cbp);
int		cbrecnext(cbase_t *cbp);
int		cbrecprev(cbase_t *cbp);
int		cbrposcmp(const void *p1, const void *p2, size_t n);
int		cbrmndx(cbase_t *cbp, int field);
int		cbsetkcur(cbase_t *cbp, int field, const cbkpos_t *cbkposp);
int		cbsetrcur(cbase_t *cbp, const cbrpos_t *cbrposp);
int		cbsync(cbase_t *cbp);
#else
int		cbclose();
int		cbcreate();
int		cbdelcur();
int		cbexport();
int		cbgetkcur();
int		cbgetlck();
int		cbgetr();
int		cbgetrcur();
int		cbgetrf();
int		cbimport();
int		cbinsert();
int		cbkeyalign();
int		cbkeyfirst();
int		cbkeylast();
int		cbkeynext();
int		cbkeyprev();
int		cbkeysrch();
int		cblock();
int		cbmkndx();
cbase_t *	cbopen();
int		cbputr();
int		cbrecalign();
int		cbrecfirst();
int		cbreclast();
int		cbrecnext();
int		cbrecprev();
int		cbrposcmp();
int		cbrmndx();
int		cbsetkcur();
int		cbsetrcur();
int		cbsync();
#endif	/* #ifdef AC_PROTO */

/* macros */
#define cbfldc(CBP)		((CBP)->fldc)
#define cbfldv(CBP)		((CBP)->fldv)
#define cbiskey(CBP, FLD)	((int)(CBP)->fldv[(FLD)].flags & CB_FKEY)
#define cbisuniq(CBP, FLD)	((int)(CBP)->fldv[(FLD)].flags & CB_FUNIQ)
#define	cbkcursor(CBP, FLD)	(btcursor((CBP)->btpv[(FLD)]))
#define	cbrcursor(CBP)		(lscursor((CBP)->lsp))
#define	cbreccnt(CBP)		((unsigned long)lsreccnt((CBP)->lsp))
#define	cbrecsize(CBP)		((size_t)lsrecsize((CBP)->lsp))

/* lock types */
#define CB_UNLCK	(0)	/* unlock */
#define CB_RDLCK	(1)	/* read lock */
#define CB_WRLCK	(2)	/* write lock */
#define CB_RDLKW	(3)	/* read lock, wait */
#define CB_WRLKW	(4)	/* write lock, wait */

/* cbase data types */
#define t_char		 (0)	/* signed character */
#define t_charv		 (1)	/* signed character vector */
#define t_uchar		 (2)	/* unsigned character */
#define t_ucharv	 (3)	/* unsigned character vector */
#define t_short		 (4)	/* signed short integer */
#define t_shortv	 (5)	/* signed short integer vector */
#define t_ushort	 (6)	/* unsigned short integer */
#define t_ushortv	 (7)	/* unsigned short integer vector */
#define t_int		 (8)	/* signed integer */
#define t_intv		 (9)	/* signed integer vector */
#define t_uint		(10)	/* unsigned integer */
#define t_uintv		(11)	/* unsigned integer vector */
#define t_long		(12)	/* long integer */
#define t_longv		(13)	/* long integer vector */
#define t_ulong		(14)	/* unsigned long integer */
#define t_ulongv	(15)	/* unsigned long integer vector */
#define t_float		(16)	/* float */
#define t_floatv	(17)	/* float vector */
#define t_double	(18)	/* double */
#define t_doublev	(19)	/* double vector */
#define t_ldouble	(20)	/* long double */
#define t_ldoublev	(21)	/* long double vector */
#define t_pointer	(22)	/* pointer */
#define t_pointerv	(23)	/* pointer vector */
#define t_string	(24)	/* character string */
#define t_cistring	(25)	/* case-insensitive character string */
#define t_binary	(26)	/* block of binary data (e.g., graphics) */

/* error codes */
#define CBEOS		(-60)		/* start of cbase error code domain */
#define CBEMFILE	(CBEOS - 1)	/* too many open cbases */
#define CBECORRUPT	(CBEOS - 2)	/* cbase is corrupt */
#define CBENOPEN	(CBEOS - 3)	/* cbase is not open */
#define CBELOCK		(CBEOS - 4)	/* incorrect lock type */
#define CBENREC		(CBEOS - 5)	/* no record */
#define CBENKEY		(CBEOS - 6)	/* no key */
#define CBEDUP		(CBEOS - 7)	/* duplicate */
#define CBEPRFILE	(CBEOS - 8)	/* printable file error */
#define CBEPANIC	(CBEOS - 9)	/* internal cbase error */

#endif		/* #ifndef H_CBASE */
