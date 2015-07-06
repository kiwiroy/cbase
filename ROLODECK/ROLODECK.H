/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)rolodeck.h	1.5 - 91/09/23" */

#ifndef H_ROLODECK	/* prevent multiple includes */
#define H_ROLODECK

/* libray headers */
#include <cbase.h>

/* record name */
#define ROLODECK	"rolodeck.dat"

/* rolodeck record definition */
typedef struct rolodeck {
	char rd_contact[40];
	char rd_title[40];
	char rd_company[40];
	char rd_addr[80];
	char rd_city[25];
	char rd_state[2];
	char rd_zip[10];
	char rd_phone[12];
	char rd_ext[4];
	char rd_fax[12];
	char rd_notes[160];
} rolodeck_t;

/* field names for record rolodeck */
#define RD_CONTACT	(0)
#define RD_TITLE	(1)
#define RD_COMPANY	(2)
#define RD_ADDR		(3)
#define RD_CITY		(4)
#define RD_STATE	(5)
#define RD_ZIP		(6)
#define RD_PHONE	(7)
#define RD_EXT		(8)
#define RD_FAX		(9)
#define RD_NOTES	(10)
#define RDFLDC		(11)

/* field definition list for record rolodeck */
extern cbfield_t rdfldv[RDFLDC];

#endif	/* #ifndef H_ROLODECK */
