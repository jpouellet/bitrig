/*	$NetBSD: mem.c,v 1.31 1996/05/03 19:42:19 christos Exp $	*/
/*	$OpenBSD: mem.c,v 1.19 2001/07/25 13:25:32 art Exp $ */
/*
 * Copyright (c) 1988 University of Utah.
 * Copyright (c) 1982, 1986, 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)mem.c	8.3 (Berkeley) 1/12/94
 */

/*
 * Memory special file
 */

#include <sys/param.h>
#include <sys/buf.h>
#include <sys/systm.h>
#include <sys/uio.h>
#include <sys/ioccom.h>
#include <sys/malloc.h>
#include <sys/memrange.h>
#include <sys/proc.h>
#include <sys/fcntl.h>

#include <machine/cpu.h>
#include <machine/conf.h>

#include <vm/vm.h>

#include <uvm/uvm_extern.h>

#include "mtrr.h"

extern char *vmmap;            /* poor name! */
caddr_t zeropage;

/* open counter for aperture */
#ifdef APERTURE
static int ap_open_count = 0;
extern int allowaperture;

#define VGA_START 0xA0000
#define BIOS_END  0xFFFFF
#endif

#if NMTRR > 0
struct mem_range_softc mem_range_softc;
static int mem_ioctl __P((dev_t, u_long, caddr_t, int, struct proc *));
#endif

/*ARGSUSED*/
int
mmopen(dev, flag, mode, p)
	dev_t dev;
	int flag, mode;
	struct proc *p;
{

	switch (minor(dev)) {
	case 0:
	case 1:
	case 2:
	case 12:
		break;
#ifdef APERTURE
	case 4:
	        if (suser(p->p_ucred, &p->p_acflag) != 0 || !allowaperture)
			return (EPERM);

		/* authorize only one simultaneous open() */
		if (ap_open_count > 0)
			return(EPERM);
		ap_open_count++;
		break;
#endif
	default:
		return (ENXIO);
	}
	return (0);
}

/*ARGSUSED*/
int
mmclose(dev, flag, mode, p)
	dev_t dev;
	int flag, mode;
	struct proc *p;
{
#ifdef APERTURE
	if (minor(dev) == 4)
		ap_open_count--;
#endif
	return (0);
}

/*ARGSUSED*/
int
mmrw(dev, uio, flags)
	dev_t dev;
	struct uio *uio;
	int flags;
{
	register vm_offset_t o, v;
	register int c;
	register struct iovec *iov;
	int error = 0;
	static int physlock;

	if (minor(dev) == 0) {
		/* lock against other uses of shared vmmap */
		while (physlock > 0) {
			physlock++;
			error = tsleep((caddr_t)&physlock, PZERO | PCATCH,
			    "mmrw", 0);
			if (error)
				return (error);
		}
		physlock = 1;
	}
	while (uio->uio_resid > 0 && error == 0) {
		iov = uio->uio_iov;
		if (iov->iov_len == 0) {
			uio->uio_iov++;
			uio->uio_iovcnt--;
			if (uio->uio_iovcnt < 0)
				panic("mmrw");
			continue;
		}
		switch (minor(dev)) {

/* minor device 0 is physical memory */
		case 0:
			v = uio->uio_offset;
			pmap_enter(pmap_kernel(), (vm_offset_t)vmmap,
			    trunc_page(v), uio->uio_rw == UIO_READ ?
			    VM_PROT_READ : VM_PROT_WRITE, PMAP_WIRED);
			o = uio->uio_offset & PGOFSET;
			c = min(uio->uio_resid, (int)(NBPG - o));
			error = uiomove((caddr_t)vmmap + o, c, uio);
			pmap_remove(pmap_kernel(), (vm_offset_t)vmmap,
			    (vm_offset_t)vmmap + NBPG);
			continue;

/* minor device 1 is kernel memory */
		case 1:
			v = uio->uio_offset;
			c = min(iov->iov_len, MAXPHYS);
			if (!uvm_kernacc((caddr_t)v, c,
			    uio->uio_rw == UIO_READ ? B_READ : B_WRITE))
				return (EFAULT);
			error = uiomove((caddr_t)v, c, uio);
			continue;

/* minor device 2 is EOF/RATHOLE */
		case 2:
			if (uio->uio_rw == UIO_WRITE)
				uio->uio_resid = 0;
			return (0);

/* minor device 12 (/dev/zero) is source of nulls on read, rathole on write */
		case 12:
			if (uio->uio_rw == UIO_WRITE) {
				c = iov->iov_len;
				break;
			}
			if (zeropage == NULL) {
				zeropage = (caddr_t)
				    malloc(PAGE_SIZE, M_TEMP, M_WAITOK);
				bzero(zeropage, PAGE_SIZE);
			}
			c = min(iov->iov_len, PAGE_SIZE);
			error = uiomove(zeropage, c, uio);
			continue;

		default:
			return (ENXIO);
		}
		if (error)
			break;
		iov->iov_base += c;
		iov->iov_len -= c;
		uio->uio_offset += c;
		uio->uio_resid -= c;
	}
	if (minor(dev) == 0) {
		if (physlock > 1)
			wakeup((caddr_t)&physlock);
		physlock = 0;
	}
	return (error);
}

int
mmmmap(dev, off, prot)
	dev_t dev;
	int off, prot;
{
	struct proc *p = curproc;	/* XXX */

	switch (minor(dev)) {
/* minor device 0 is physical memory */
	case 0:
		if ((u_int)off > ctob(physmem) &&
		    suser(p->p_ucred, &p->p_acflag) != 0)
			return -1;
		return i386_btop((u_int)off);

/* minor device 1 is kernel memory */
	case 1:
		/* XXX - writability, executability checks? */
		if (!uvm_kernacc((caddr_t)off, NBPG, B_READ))
			return -1;
		return i386_btop(vtophys(off));
#ifdef APERTURE
/* minor device 4 is aperture driver */
	case 4:
		switch (allowaperture) {
		case 1:
			/* Allow mapping of the VGA framebuffer & BIOS only */
			if ((off >= VGA_START && off <= BIOS_END) ||
			    (unsigned)off > (unsigned)ctob(physmem))
				return i386_btop(off);
			else
				return -1;
		case 2:
			/* Allow mapping of the whole 1st megabyte 
			   for x86emu */
			if (off <= BIOS_END || 
			    (unsigned)off > (unsigned)ctob(physmem))
				return i386_btop(off);
			else 
				return -1;
		default:
			return -1;
		}
			
#endif
	default:
		return -1;
	}
}

int
mmioctl(dev, cmd, data, flags, p)
	dev_t dev;
	u_long cmd;
	caddr_t data;
	int flags;
	struct proc *p;
{
#if NMTRR > 0
	switch (minor(dev)) {
	case 0:
	case 4:
		return mem_ioctl(dev, cmd, data, flags, p);
	}
#endif
	return (ENODEV);
}

#if NMTRR > 0
/*
 * Operations for changing memory attributes.
 *
 * This is basically just an ioctl shim for mem_range_attr_get
 * and mem_range_attr_set.
 */
static int 
mem_ioctl(dev, cmd, data, flags, p)
	dev_t dev;
	u_long cmd;
	caddr_t data;
	int flags;
	struct proc *p;
{
	int nd, error = 0;
	struct mem_range_op *mo = (struct mem_range_op *)data;
	struct mem_range_desc *md;
	
	/* is this for us? */
	if ((cmd != MEMRANGE_GET) &&
	    (cmd != MEMRANGE_SET))
		return (ENOTTY);

	/* any chance we can handle this? */
	if (mem_range_softc.mr_op == NULL)
		return (EOPNOTSUPP);

	/* do we have any descriptors? */
	if (mem_range_softc.mr_ndesc == 0)
		return (ENXIO);

	switch (cmd) {
	case MEMRANGE_GET:
		nd = imin(mo->mo_arg[0], mem_range_softc.mr_ndesc);
		if (nd > 0) {
			md = (struct mem_range_desc *)
				malloc(nd * sizeof(struct mem_range_desc),
				       M_MEMDESC, M_WAITOK);
			error = mem_range_attr_get(md, &nd);
			if (!error)
				error = copyout(md, mo->mo_desc, 
					nd * sizeof(struct mem_range_desc));
			free(md, M_MEMDESC);
		} else {
			nd = mem_range_softc.mr_ndesc;
		}
		mo->mo_arg[0] = nd;
		break;
		
	case MEMRANGE_SET:
		md = (struct mem_range_desc *)malloc(sizeof(struct mem_range_desc),
						    M_MEMDESC, M_WAITOK);
		error = copyin(mo->mo_desc, md, sizeof(struct mem_range_desc));
		/* clamp description string */
		md->mr_owner[sizeof(md->mr_owner) - 1] = 0;
		if (error == 0)
			error = mem_range_attr_set(md, &mo->mo_arg[0]);
		free(md, M_MEMDESC);
		break;
	}
	return (error);
}

/*
 * Implementation-neutral, kernel-callable functions for manipulating
 * memory range attributes.
 */
int
mem_range_attr_get(mrd, arg)
	struct mem_range_desc *mrd;
	int *arg;
{
	/* can we handle this? */
	if (mem_range_softc.mr_op == NULL)
		return (EOPNOTSUPP);

	if (*arg == 0) {
		*arg = mem_range_softc.mr_ndesc;
	} else {
		bcopy(mem_range_softc.mr_desc, mrd, (*arg) * sizeof(struct mem_range_desc));
	}
	return (0);
}

int
mem_range_attr_set(mrd, arg)
	struct mem_range_desc *mrd;
	int *arg;
{
	/* can we handle this? */
	if (mem_range_softc.mr_op == NULL)
		return (EOPNOTSUPP);

	return (mem_range_softc.mr_op->set(&mem_range_softc, mrd, arg));
}

#endif /* NMTRR > 0 */

