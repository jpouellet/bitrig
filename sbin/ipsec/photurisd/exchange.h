/*
 * Copyright 1997 Niels Provos <provos@physnet.uni-hamburg.de>
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
 *      This product includes software developed by Niels Provos.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/* $Id: exchange.h,v 1.1 1997/07/18 22:48:49 provos Exp $ */
/* 
 * exchange.h: 
 * exchange generation header file
 */ 

#ifndef _EXCHANGE_H_
#define _EXCHANGE_H_
 
#undef EXTERN

#ifdef _EXCHANGE_C_
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void make_random_mpz(mpz_t a, mpz_t bits);
EXTERN u_int8_t *varpre_get_number_bits(mpz_t bits, u_int8_t *varpre);
EXTERN u_int8_t *mpz_set_varpre(mpz_t a, u_int8_t *varpre);
EXTERN u_int8_t *mpz_init_set_varpre(mpz_t a, u_int8_t *varpre);
EXTERN void mpz_get_number_bits(mpz_t rop, mpz_t p);
EXTERN int mpz_to_varpre(u_int8_t *value, u_int16_t *size, mpz_t p, mpz_t bits);
EXTERN int exchange_make_values(struct stateob *, mpz_t, mpz_t );
EXTERN int exchange_value_generate(struct stateob *, u_int8_t *, u_int16_t *);

#endif
