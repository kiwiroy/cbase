/*	Copyright (c) 1991 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"@(#)rolodeck.i	1.5 - 91/09/23" */

#ifndef I_ROLODECK	/* prevent multiple includes */
#define I_ROLODECK

#include <ansi.h>

/* ansi headers */
#ifdef AC_STDDEF
#include <stddef.h>
#endif

/* libray headers */
#include <cbase.h>

/* local headers */
#include "rolodeck.h"

/* field definition list for record rolodeck */
cbfield_t rdfldv[] = {
	{
		offsetof(struct rolodeck, rd_contact),
		sizeofm(struct rolodeck, rd_contact),
		t_string,
		CB_FKEY | CB_FUNIQ,
		"rdcont.ndx"
	},
	{
		offsetof(struct rolodeck, rd_title),
		sizeofm(struct rolodeck, rd_title),
		t_string,
		0,
		NULL
	},
	{
		offsetof(struct rolodeck, rd_company),
		sizeofm(struct rolodeck, rd_company),
		t_string,
		CB_FKEY,
		"rdcomp.ndx"
	},
	{
		offsetof(struct rolodeck, rd_addr),
		sizeofm(struct rolodeck, rd_addr),
		t_string,
		0,
		NULL
	},
	{
		offsetof(struct rolodeck, rd_city),
		sizeofm(struct rolodeck, rd_city),
		t_string,
		0,
		NULL
	},
	{
		offsetof(struct rolodeck, rd_state),
		sizeofm(struct rolodeck, rd_state),
		t_string,
		0,
		NULL
	},
	{
		offsetof(struct rolodeck, rd_zip),
		sizeofm(struct rolodeck, rd_zip),
		t_string,
		0,
		NULL
	},
	{
		offsetof(struct rolodeck, rd_phone),
		sizeofm(struct rolodeck, rd_phone),
		t_string,
		0,
		NULL
	},
	{
		offsetof(struct rolodeck, rd_ext),
		sizeofm(struct rolodeck, rd_ext),
		t_string,
		0,
		NULL
	},
	{
		offsetof(struct rolodeck, rd_fax),
		sizeofm(struct rolodeck, rd_fax),
		t_string,
		0,
		NULL
	},
	{
		offsetof(struct rolodeck, rd_notes),
		sizeofm(struct rolodeck, rd_notes),
		t_string,
		0,
		NULL
	},
};

#endif	/* #ifndef I_ROLODECK */
