/* $OpenBSD: reloc.h,v 1.1 2004/02/10 14:30:05 drahn Exp $ */

/*
 * Copyright (c) 2002,2003 Dale Rahn
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
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
#ifndef  _MACH_RELOC_H 
#define  _MACH_RELOC_H 
#define R_TYPE(X)       __CONCAT(R_X86_64_,X)

#define R_X86_64_NONE		0
#define R_X86_64_64		1
#define R_X86_64_PC32		2
#define R_X86_64_GOT32		3
#define R_X86_64_PLT32		4
#define R_X86_64_COPY		5
#define R_X86_64_GLOB_DAT	6
#define R_X86_64_JUMP_SLOT	7
#define R_X86_64_RELATIVE	8
#define R_X86_64_GOTPCREL	9
#define R_X86_64_32		10
#define R_X86_64_32S		11
#define R_X86_64_16		12
#define R_X86_64_PC16		13
#define R_X86_64_8		14
#define R_X86_64_PC8		15
#define R_X86_64_DPTMOD64	16
#define R_X86_64_DTPOFF64	17
#define R_X86_64_TPOFF64	18
#define R_X86_64_TLSGD		19
#define R_X86_64_TLSLD		20
#define R_X86_64_DTPOFF32	21
#define R_X86_64_GOTTPOFF	22
#define R_X86_64_TPOFF32	23

#endif /* _MACH_RELOC_H */
