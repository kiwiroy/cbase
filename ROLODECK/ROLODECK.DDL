/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)rolodeck.ddl	1.5 - 91/09/23" */

/* file assignments */
data file	"rolodeck.dat"	contains rolodeck;
index file	"rdcont.ndx"	contains rd_contact;
index file	"rdcomp.ndx"	contains rd_company;

/* record definitions */
record rolodeck {				/* rolodeck record */
	unique key t_string	rd_contact[40];	/* contact name */
	t_string		rd_title[40];	/* contact title */
	key t_string		rd_company[40];	/* company name */
	t_string		rd_addr[80];	/* address */
	t_string		rd_city[25];	/* city */
	t_string		rd_state[2];	/* state */
	t_string		rd_zip[10];	/* zip code */
	t_string		rd_phone[12];	/* phone number */
	t_string		rd_ext[4];	/* phone extension */
	t_string		rd_fax[12];	/* fax number */
	t_string		rd_notes[160];	/* notes */
};
