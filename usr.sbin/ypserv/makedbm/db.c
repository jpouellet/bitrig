/*	$OpenBSD: db.c,v 1.2 2001/10/24 14:40:00 deraadt Exp $ */

/*
 * Copyright (c) 1997 Mats O Jansson <moj@stacken.kth.se>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Mats O Jansson
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef LINT
static char rcsid[] = "$OpenBSD: db.c,v 1.2 2001/10/24 14:40:00 deraadt Exp $";
#endif

#include <sys/types.h>
#include <db.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/param.h>
#include "db.h"
#include "ypdb.h"

/*
 * This module was created to be able to read database files created
 * by sendmail -bi.
 */

int db_hash_list_database(database)
char *database;
{
	DB *db;
	int  status;
	DBT key, val;
	char path[MAXPATHLEN];

	snprintf(path, sizeof(path), "%s%s", database, ".db");

	db = dbopen(path, O_RDONLY, 0, DB_HASH, NULL);
	if (db != NULL) {
		status = db->seq(db, &key, &val, R_FIRST);
		while (status == 0) {
			printf("%*.*s %*.*s\n",
			    key.size-1, key.size-1, key.data,
			    val.size-1, val.size-1, val.data);
			status = db->seq(db, &key, &val, R_NEXT);
		}
		db->close(db);
		return(1);
	}
	return(0);
}

