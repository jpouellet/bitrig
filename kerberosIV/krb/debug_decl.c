/*	$OpenBSD: debug_decl.c,v 1.3 1997/12/09 07:57:12 art Exp $	*/
/* $KTH: debug_decl.c,v 1.7 1997/10/28 15:44:00 bg Exp $ */

/* 
  Copyright (C) 1989 by the Massachusetts Institute of Technology

   Export of this software from the United States of America is assumed
   to require a specific license from the United States Government.
   It is the responsibility of any person or organization contemplating
   export to obtain such a license before exporting.

WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
distribute this software and its documentation for any purpose and
without fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright notice and
this permission notice appear in supporting documentation, and that
the name of M.I.T. not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.  M.I.T. makes no representations about the suitability of
this software for any purpose.  It is provided "as is" without express
or implied warranty.

  */

#include "krb_locl.h"

/* Declare global debugging variables. */

int krb_ap_req_debug = 0;
int krb_debug = 0;
int krb_dns_debug = 0;
