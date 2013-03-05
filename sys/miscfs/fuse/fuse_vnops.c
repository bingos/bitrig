/*
 * Copyright (c) 2012-2013 Sylvestre Gallon <ccna.syl@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/vnode.h>
#include <sys/proc.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/dirent.h>
#include <sys/specdev.h>
#include <sys/poll.h>
#include <sys/lockf.h>

#include "fuse_kernel.h"
#include "fuse_node.h"
#include "fusefs.h"

/*
 * Prototypes for fusefs vnode ops
 */
extern int fusefs_lookup(void *);
int	fusefs_open(void *);
int	fusefs_close(void *);
int	fusefs_access(void *);
int	fusefs_getattr(void *);
int	fusefs_setattr(void *);
int	fusefs_ioctl(void *);
int	fusefs_link(void *);
int	fusefs_symlink(void *);
int	fusefs_readdir(void *);
int	fusefs_readlink(void *);
int	fusefs_inactive(void *);
int	fusefs_reclaim(void *);
int	fusefs_print(void *);
int	fusefs_create(void *);
int	fusefs_mknod(void *);
int	fusefs_read(void *);
int	fusefs_write(void *);
int	fusefs_poll(void *);
int	fusefs_fsync(void *);
int	fusefs_remove(void *);
int	fusefs_rename(void *);
int	fusefs_mkdir(void *);
int	fusefs_rmdir(void *);
int	fusefs_strategy(void *);
int	fusefs_lock(void *);
int	fusefs_unlock(void *);
int	fusefs_islocked(void *);
int	fusefs_advlock(void *);
int	readdir_process_data(void *buff, int len, struct uio *uio);

struct vops fusefs_vops = {
	.vop_lookup	= fusefs_lookup,
	.vop_create	= fusefs_create,
	.vop_mknod	= fusefs_mknod,
	.vop_open	= fusefs_open,
	.vop_close	= fusefs_close,
	.vop_access	= fusefs_access,
	.vop_getattr	= fusefs_getattr,
	.vop_setattr	= fusefs_setattr,
	.vop_read	= fusefs_read,
	.vop_write	= fusefs_write,
	.vop_ioctl	= fusefs_ioctl,
	.vop_poll	= fusefs_poll,
	.vop_fsync	= fusefs_fsync,
	.vop_remove	= fusefs_remove,
	.vop_link	= fusefs_link,
	.vop_rename	= fusefs_rename,
	.vop_mkdir	= fusefs_mkdir,
	.vop_rmdir	= fusefs_rmdir,
	.vop_symlink	= fusefs_symlink,
	.vop_readdir	= fusefs_readdir,
	.vop_readlink	= fusefs_readlink,
	.vop_abortop	= vop_generic_abortop,
	.vop_inactive	= fusefs_inactive,
	.vop_reclaim	= fusefs_reclaim,
	.vop_lock	= fusefs_lock,
	.vop_unlock	= fusefs_unlock,
	.vop_bmap	= vop_generic_bmap,
	.vop_strategy	= fusefs_strategy,
	.vop_print	= fusefs_print,
	.vop_islocked	= fusefs_islocked,
	.vop_pathconf	= spec_pathconf,
	.vop_advlock	= fusefs_advlock,
};


void
fuse_internal_attr_fat2vat(struct mount *mp,
                           struct fuse_attr *fat,
                           struct vattr *vap)
{
	vap->va_fsid = mp->mnt_stat.f_fsid.val[0];
	vap->va_fileid = fat->ino;/* XXX cast from 64 bits to 32 */
	vap->va_mode = fat->mode & ~S_IFMT;
	vap->va_nlink = fat->nlink;
	vap->va_uid = fat->uid;
	vap->va_gid = fat->gid;
	vap->va_rdev = fat->rdev;
	vap->va_size = fat->size;
	vap->va_atime.tv_sec = fat->atime;
	vap->va_atime.tv_nsec = fat->atimensec;
	vap->va_mtime.tv_sec = fat->mtime;
	vap->va_mtime.tv_nsec = fat->mtimensec;
	vap->va_ctime.tv_sec = fat->ctime;
	vap->va_ctime.tv_nsec = fat->ctimensec;
	vap->va_blocksize = PAGE_SIZE;
	vap->va_type = IFTOVT(fat->mode);

#if (S_BLKSIZE == 512)
	/* Optimize this case */
	vap->va_bytes = fat->blocks << 9;
#else
	vap->va_bytes = fat->blocks * S_BLKSIZE;
#endif

}

int
fusefs_open(void *v)
{
	struct vop_open_args *ap;
	struct fuse_node *ip;
	struct fuse_mnt *fmp;
	enum fufh_type fufh_type = FUFH_RDONLY;
	int flags;
	int error;
	int isdir;

#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_open\n");
#endif
	ap = v;
	ip = VTOI(ap->a_vp);
	fmp = ip->i_mnt;

	if (!fmp->sess_init) {
		return (0);
	}

#ifdef FUSE_DEBUG_VNOP
	printf("inode = %i mode=0x%x\n", ip->i_number, ap->a_mode);
#endif
	isdir = 0;
	if (ip->vtype == VDIR) {
		fufh_type = FUFH_RDONLY; 
		flags = O_RDONLY;
		isdir = 1;
	} else {
		if ((ap->a_mode & FREAD) && (ap->a_mode & FWRITE)) {
			fufh_type = FUFH_RDWR;
			flags = O_RDWR;
		}
		else if (ap->a_mode  & (FWRITE)) {
			fufh_type = FUFH_WRONLY;
			flags = O_WRONLY;
		}
		else if (ap->a_mode & (FREAD)) {
			fufh_type = FUFH_RDONLY;
			flags = O_RDONLY;
		}
	}

	/* already open i think all is ok */
	if (ip->fufh[fufh_type].fh_type != FUFH_INVALID)
		return 0;

	error = fuse_file_open(fmp, ip, fufh_type, flags, isdir);

	if (error)
		return (error);

#ifdef FUSE_DEBUG_VNOP
	printf("file open fd : %i\n", ip->fufh[fufh_type].fh_id);
#endif
	return (error);
}

int
fusefs_close(void *v)
{
	struct vop_close_args *ap;
	struct fuse_node *ip;
	struct fuse_mnt *fmp;
	enum fufh_type fufh_type = FUFH_RDONLY;
	int isdir, flags, i;

#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_close\n");
#endif

	ap = v;
	ip = VTOI(ap->a_vp);
	fmp = ip->i_mnt;

	if (!fmp->sess_init) {
		return (0);
	}

	isdir = 0;
	if (ip->vtype == VDIR) {
		fufh_type = FUFH_RDONLY; 
		isdir = 1;
		
		if (ip->fufh[fufh_type].fh_type != FUFH_INVALID) 
			return fuse_file_close(fmp, ip, fufh_type, O_RDONLY, isdir);
	} else {
		if (ap->a_fflag & IO_NDELAY) {
			return (0);
		}

		if ((ap->a_fflag & FREAD) && (ap->a_fflag & FWRITE)) {
			fufh_type = FUFH_RDWR;
			flags = O_RDWR;
		}
		else if (ap->a_fflag  & (FWRITE)) {
			fufh_type = FUFH_WRONLY;
			flags = O_WRONLY;
		}
		else if (ap->a_fflag & (FREAD)) {
			fufh_type = FUFH_RDONLY;
			flags = O_RDONLY;
		}
	}

	/* 
	 * if fh not valid lookup for another valid fh in vnode.
	 * panic if there's not fh valid
	 */
	if (ip->fufh[fufh_type].fh_type != FUFH_INVALID) {
		for (i = 0; i < FUFH_MAXTYPE; i++)
			if (ip->fufh[fufh_type].fh_type != FUFH_INVALID)
				break;
		return (0);
	}

	return (0);
}

int
fusefs_access(void *v)
{
	struct fuse_access_in access;
	struct vop_access_args *ap;
	struct fuse_in_header hdr;
	struct fuse_node *ip;
	struct fuse_mnt *fmp;
	struct fuse_msg msg;
	uint32_t mask = 0;
	int error = 0;

#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_access\n");
#endif
	ap = v;
	ip = VTOI(ap->a_vp);
	fmp = ip->i_mnt;

	if (!fmp->sess_init || (fmp->undef_op & UNDEF_ACCESS))
		goto system_check;

	if (ap->a_vp->v_type == VLNK) 
		goto system_check;

	if (ap->a_vp->v_type == VREG && (ap->a_mode & VWRITE & VEXEC))
		goto system_check;

	if ((ap->a_mode & VWRITE) && (fmp->mp->mnt_flag & MNT_RDONLY))
		return(EACCES);

	if ((ap->a_mode & VWRITE) != 0) {
		mask |= 0x2;
	}
	if ((ap->a_mode & VREAD) != 0) {
		mask |= 0x4;
	}
	if ((ap->a_mode & VEXEC) != 0) {
		mask |= 0x1;
	}

	bzero(&msg, sizeof(msg));
	msg.hdr = &hdr;
	access.mask = mask;
	msg.data = &access;
	msg.len = sizeof(access);
	msg.cb = &fuse_sync_it;
	msg.fmp = fmp;
	msg.type = msg_intr;

	fuse_make_in(fmp->mp, msg.hdr, msg.len, FUSE_ACCESS, ip->i_number, curproc);
	
	TAILQ_INSERT_TAIL(&fmq_in, &msg, node);
	wakeup(&fmq_in);

	error = tsleep(&msg, PWAIT, "fuse access", 0);

	if (error)
		return error;

#ifdef FUSE_DEBUG_VNOP
	printf("it with value %x\n", msg.rep.it_res);
#endif
	error = msg.rep.it_res;

	if (error == ENOSYS) {
		fmp->undef_op |= UNDEF_ACCESS;
		goto system_check;
	}

	return (error);

system_check:
#ifdef FUSE_DEBUG_VNOP
	printf("Use kernel access\n");
#endif
	return (vaccess(ap->a_vp->v_type, ip->cached_attrs.va_mode & ALLPERMS,
		ip->cached_attrs.va_uid, ip->cached_attrs.va_gid, ap->a_mode, ap->a_cred));
}

int
fusefs_getattr(void *v)
{
  	struct vop_getattr_args *ap = v;
	struct vnode *vp = ap->a_vp;
	struct fuse_mnt *fmp;
	struct vattr *vap = ap->a_vap;
	struct fuse_in_header hdr;
	struct fuse_attr_out *fat;
	struct fuse_node *ip;
	struct fuse_msg msg;
	int error = 0;

#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_getattr\n");
#endif
	ip = VTOI(vp);
	fmp = ip->i_mnt;
	
	if (!fmp->sess_init) {
		goto fake;
	}

	bzero(&msg, sizeof(msg));
	msg.hdr = &hdr;
	msg.len = 0;
	msg.data = NULL;
	msg.rep.buff.len = sizeof(*fat);
	msg.rep.buff.data_rcv = NULL;
	msg.type = msg_buff;
	msg.cb = &fuse_sync_resp;
	
	fuse_make_in(fmp->mp, msg.hdr, msg.len, FUSE_GETATTR, ip->i_number, curproc);
	
	TAILQ_INSERT_TAIL(&fmq_in, &msg, node);
	wakeup(&fmq_in);

	error = tsleep(&msg, PWAIT, "fuse getattr", 0);
	
	if (error) {
		if (msg.rep.buff.data_rcv != NULL)
			free(msg.rep.buff.data_rcv, M_FUSEFS);
		return (error);
	}
	
	fat = (struct fuse_attr_out *)msg.rep.buff.data_rcv;
	
#ifdef FUSE_DEBUG_VNOP
	printf("ino: %d\n", fat->attr.ino);
	printf("size: %d\n", fat->attr.size);
	printf("blocks: %d\n", fat->attr.blocks);
	printf("atime: %d\n", fat->attr.atime);
	printf("mtime: %d\n", fat->attr.mtime);
	printf("ctime: %d\n", fat->attr.ctime);
	printf("atimensec: %d\n", fat->attr.atimensec);
	printf("mtimensec: %d\n", fat->attr.mtimensec);
	printf("ctimensec: %d\n", fat->attr.ctimensec);
	printf("mode: %d\n", fat->attr.mode);
	printf("nlink: %d\n", fat->attr.nlink);
	printf("uid: %d\n", fat->attr.uid);
	printf("gid: %d\n", fat->attr.gid);
	printf("rdev: %d\n", fat->attr.rdev);
#endif

	fuse_internal_attr_fat2vat(fmp->mp, &fat->attr, vap);

	memcpy(&ip->cached_attrs, vap, sizeof(*vap));	
	free(fat, M_FUSEFS);

	return (error);
fake:
	bzero(vap, sizeof(*vap));
	vap->va_type = vp->v_type;
	return (0);
}

int
fusefs_setattr(void *v)
{
#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_setattr\n");
#endif
	return (0);
}

int
fusefs_ioctl(void *v)
{
#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_ioctl\n");
#endif
	return (ENOTTY);
}

int
fusefs_link(void *v)
{
	struct vop_link_args *ap = v;
	struct vnode *dvp = ap->a_dvp;
	struct vnode *vp = ap->a_vp;
	struct componentname *cnp = ap->a_cnp;
	struct fuse_in_header hdr;
	struct fuse_entry_out *feo;
	struct fuse_link_in fli;
	struct fuse_mnt *fmp;
	struct fuse_node *ip;
	struct fuse_msg msg;
	int error = 0;

#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_link\n");
#endif

	if (vp->v_type == VDIR) {
		VOP_ABORTOP(dvp, cnp);
		error = EISDIR;
		goto out2;
	}
	if (dvp->v_mount != vp->v_mount) {
		VOP_ABORTOP(dvp, cnp);
		error = EXDEV;
		goto out2;
	}
	if (dvp != vp && (error = vn_lock(vp, LK_EXCLUSIVE, curproc))) {
		VOP_ABORTOP(dvp, cnp);
		goto out2;
	}

	ip = VTOI(vp);
	fmp = ip->i_mnt;

	if (!fmp->sess_init || (fmp->undef_op & UNDEF_LINK))
		goto out1;

	fli.oldnodeid = ip->i_number;

	bzero(&msg, sizeof(msg));
	msg.hdr = &hdr;
	msg.len = sizeof(fli);
	msg.data = &fli;
	msg.rep.buff.len = sizeof(struct fuse_entry_out);
	msg.rep.buff.data_rcv = NULL;
	msg.type = msg_buff;
	msg.cb = &fuse_sync_resp;

	fuse_make_in(fmp->mp, msg.hdr, msg.len, FUSE_LINK, ip->i_number, curproc);

	TAILQ_INSERT_TAIL(&fmq_in, &msg, node);
	wakeup(&fmq_in);

	error = tsleep(&msg, PWAIT, "fuse link", 0);
	
	if (error) {
		if (msg.rep.buff.data_rcv != NULL)
			free(msg.rep.buff.data_rcv, M_FUSEFS);
		goto out1;
	}

	if (msg.error) {
		error = msg.error;

		if (error == ENOSYS) {
			fmp->undef_op |= UNDEF_LINK;
		}

		goto out1;
	}

	feo = (struct fuse_entry_out *)msg.rep.buff.data_rcv;
	free(feo, M_FUSEFS);

out1:
	if (dvp != vp)
		VOP_UNLOCK(vp, 0);
out2:
	vput(dvp);
	return (error);
}

int
fusefs_symlink(void *v)
{
#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_symlink\n");
#endif
	return (0);
}

#define GENERIC_DIRSIZ(dp) \
    ((sizeof (struct dirent) - (MAXNAMLEN+1)) + (((dp)->d_namlen+1 + 3) &~ 3))


int
readdir_process_data(void *buff, int len, struct uio *uio)
{
	struct fuse_dirent *fdir;
	struct dirent dir;
	int bytesavail;
	int flen, error = 0;

	if (len < FUSE_NAME_OFFSET) {
		return 0;
	}

	while ( len > 0) {
		if ( len < sizeof(*fdir)) {
			error = 0;
			break;
		}

		fdir = (struct fuse_dirent *)buff;
		flen = FUSE_DIRENT_SIZE(fdir);

		if (!fdir->namelen || fdir->namelen > MAXNAMLEN || len < flen) {
			error = EINVAL;
			break;
		}

		bytesavail = GENERIC_DIRSIZ((struct pseudo_dirent *) &fdir->namelen);
		if (bytesavail > uio->uio_resid) {
			error = 0;
			break;
		}

		bzero(&dir, sizeof(dir));
		dir.d_fileno = fdir->ino;
		dir.d_reclen = bytesavail;
		dir.d_type = fdir->type;
		dir.d_namlen = fdir->namelen;
		bcopy(fdir->name, dir.d_name, fdir->namelen);

		uiomove(&dir, bytesavail , uio);
		len -= flen;
		/* ugly pointer arithmetic must find something else ...*/
		buff = (void *)(((char *) buff) + flen);
		uio->uio_offset = fdir->off;
	}
	
	return (error);
}

int
fusefs_readdir(void *v)
{
	struct vop_readdir_args *ap = v;
	struct fuse_read_in read;
	struct fuse_in_header hdr;
	struct fuse_node *ip;
	struct fuse_mnt *fmp;
	struct fuse_msg msg;
	struct vnode *vp;
	struct uio *uio;
	int error = 0;

	vp = ap->a_vp;
	uio = ap->a_uio;
	ip = VTOI(vp);
	fmp = ip->i_mnt;

	if (!fmp->sess_init) {
		return (0);
	}

#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_readdir\n");
	printf("uio resid 0x%x\n", uio->uio_resid);
#endif
	if (uio->uio_resid == 0)
		return (error);

	while (uio->uio_resid > 0) {
		bzero(&msg, sizeof(msg));
		msg.hdr = &hdr;
		msg.len = sizeof(read);
		msg.type = msg_buff;

		msg.rep.buff.len= 0;
		msg.rep.buff.data_rcv = NULL;
		msg.data = &read;
		msg.cb = &fuse_sync_resp;

		if (ip->fufh[FUFH_RDONLY].fh_type == FUFH_INVALID) {
		      printf("dir not open\n");
		      /* TODO open the file */
		      return (error);
		}
		read.fh = ip->fufh[FUFH_RDONLY].fh_id;
		read.offset = uio->uio_offset;
		read.size = MIN(uio->uio_resid, PAGE_SIZE);

		fuse_make_in(fmp->mp, msg.hdr, msg.len, FUSE_READDIR, ip->i_number, curproc);
		
		TAILQ_INSERT_TAIL(&fmq_in, &msg, node);
		wakeup(&fmq_in);

		error = tsleep(&msg, PWAIT, "fuse getattr", 0);

		if (error) {
			if (msg.rep.buff.len != 0)
				free(msg.rep.buff.data_rcv, M_FUSEFS);
			return (error);
		}

		if (msg.error == -1) {
			//ack for end of readdir
			break;
		}

		if ((error = readdir_process_data(msg.rep.buff.data_rcv, msg.rep.buff.len, uio))) {
			if (msg.rep.buff.len != 0)
				free(msg.rep.buff.data_rcv, M_FUSEFS);
			break;
		}

		if (msg.rep.buff.len != 0)
			free(msg.rep.buff.data_rcv, M_FUSEFS);
	}

	return (error);
}

int
fusefs_inactive(void *v)
{
	struct vop_inactive_args *ap = v;
	struct vnode *vp = ap->a_vp;
	struct proc *p = curproc;
	register struct fuse_node *ip = VTOI(vp);
	int error = 0;

#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_inactive\n");
#endif
	
	ip->flag = 0;
	VOP_UNLOCK(vp, 0);

	/* not sure if it is ok to do like that ...*/
	if (ip->cached_attrs.va_mode == 0)
		vrecycle(vp, p); 

	return (error);
}

int
fusefs_readlink(void *v)
{
  	struct vop_readlink_args *ap = v;
	struct vnode *vp = ap->a_vp;
	struct fuse_in_header hdr;
	struct fuse_node *ip;
	struct fuse_mnt *fmp;
	struct fuse_msg msg;
	struct	uio *uio;
	int error = 0;

#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_readlink\n");
#endif
	ip = VTOI(vp);
	fmp = ip->i_mnt;
	uio = ap->a_uio;

	if (!fmp->sess_init || (fmp->undef_op & UNDEF_READLINK)) {
		error = ENOSYS;
		goto out;
	}

	bzero(&msg, sizeof(msg));
	msg.hdr = &hdr;
	msg.len = 0;
	msg.data = NULL;
	msg.type = msg_buff;
	msg.rep.buff.len = 0;
	msg.rep.buff.data_rcv = NULL;
	msg.cb = &fuse_sync_resp;

	fuse_make_in(fmp->mp, msg.hdr, msg.len, FUSE_READLINK, ip->i_number, curproc);

	TAILQ_INSERT_TAIL(&fmq_in, &msg, node);
	wakeup(&fmq_in);

	if ((error = tsleep(&msg, PWAIT, "fuse readlink", 0)))
		goto out;

	if (msg.error) {
		error = msg.error;

		if (error == ENOSYS) {
			fmp->undef_op |= UNDEF_READLINK;
		}
		goto out;
	}

	error = uiomove(msg.rep.buff.data_rcv, msg.rep.buff.len, uio);

out:

	if (msg.rep.buff.data_rcv)
		free(msg.rep.buff.data_rcv, M_FUSEFS);
	return (error);
}

int
fusefs_reclaim(void *v)
{
	struct vop_reclaim_args *ap = v;
	struct vnode *vp = ap->a_vp;
	struct fuse_node *ip = VTOI(vp);

#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_reclaim\n");
#endif
	/*
	 * Purge old data structures associated with the inode.
	 */
	ip->parent = 0;

	/*
	 * Remove the inode from its hash chain.
	 */
	fusefs_ihashrem(ip);
	cache_purge(vp);

	/*close file if exist
	if (ip->i_fd) {
		vrele(ip->i_devvp);
		ip->i_devvp = 0;
	}*/

	free(ip, M_FUSEFS);
	vp->v_data = NULL;
	return (0);
}

int
fusefs_print(void *v)
{
	struct vop_print_args *ap = v;
	struct vnode *vp = ap->a_vp;
	struct fuse_node *ip = VTOI(vp);

	/*
	 * Complete the information given by vprint().
	 */
	printf("tag VT_FUSE, hash id %u ", ip->i_number);
	lockmgr_printinfo(&ip->i_lock);
	printf("\n");
	return (0);
}

int
fusefs_create(void *v)
{
	struct vop_create_args *ap = v;
	struct componentname *cnp = ap->a_cnp;
	struct vnode **vpp = ap->a_vpp;
	struct vnode *dvp = ap->a_dvp;
	struct vattr *vap = ap->a_vap;
	struct fuse_entry_out *feo;
	struct fuse_in_header hdr;
	struct vnode *tdp = NULL;
	struct fuse_open_in foi;
	struct fuse_mnt *fmp;
	struct fuse_node *ip;
	struct fuse_msg msg;
	int error = 0;
	mode_t mode;

#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_create(cnp %08x, vap %08x\n", cnp, vap);
#endif
	ip = VTOI(dvp);
	fmp = ip->i_mnt;
	mode = MAKEIMODE(vap->va_type, vap->va_mode);

	if (!fmp->sess_init || (fmp->undef_op & UNDEF_CREATE)) {
		error = ENOSYS;
		goto out;
	}

	bzero(&msg, sizeof(msg));
	msg.hdr = &hdr;
	msg.len = sizeof(foi) + cnp->cn_namelen + 1;
	msg.data = malloc(msg.len, M_FUSEFS, M_WAITOK | M_ZERO);
	msg.type = msg_buff;
	msg.rep.buff.len = sizeof(struct fuse_entry_out) + sizeof(struct fuse_open_out);
	msg.rep.buff.data_rcv = NULL;
	msg.cb = &fuse_sync_resp;

	foi.mode = mode;
	foi.flags = O_CREAT | O_RDWR;

	memcpy(msg.data, &foi, sizeof(foi));
	memcpy((char *)msg.data + sizeof(foi), cnp->cn_nameptr, cnp->cn_namelen);
	((char *)msg.data)[sizeof(foi) + cnp->cn_namelen] = '\0';

	fuse_make_in(fmp->mp, msg.hdr, msg.len, FUSE_CREATE, ip->i_number, curproc);

	TAILQ_INSERT_TAIL(&fmq_in, &msg, node);
	wakeup(&fmq_in);

	if ((error = tsleep(&msg, PWAIT, "fuse create", 0)))
		goto out;

	if (msg.error) {
		error = msg.error;

		if (error == ENOSYS) {
			fmp->undef_op |= UNDEF_CREATE;
		}
		goto out;
	}

	feo = msg.rep.buff.data_rcv;
	if ((error = VFS_VGET(fmp->mp, feo->nodeid, &tdp)))
		goto out;

	tdp->v_type = IFTOVT(feo->attr.mode);
	VTOI(tdp)->vtype = tdp->v_type;

	if (dvp != NULL && dvp->v_type == VDIR)
		VTOI(tdp)->parent = ip->i_number;

	*vpp = tdp;
	VN_KNOTE(ap->a_dvp, NOTE_WRITE);

out:
	vput(ap->a_dvp);

	if (msg.rep.buff.data_rcv)
		free(msg.rep.buff.data_rcv, M_FUSEFS);

	return (error);
}

int
fusefs_mknod(void *v)
{
	struct vop_mknod_args *ap = v;
  
	VN_KNOTE(ap->a_dvp, NOTE_WRITE);
	vput(ap->a_dvp);
	return (EINVAL);
}

int
fusefs_read(void *v)
{
	struct vop_read_args *ap = v;
	struct vnode *vp = ap->a_vp;
	struct uio *uio = ap->a_uio;
	struct fuse_in_header hdr;
	struct fuse_read_in fri;
	struct fuse_node *ip;
	struct fuse_mnt *fmp;
	struct fuse_msg msg;
	int error=0;

#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_read\n");
#endif
	ip = VTOI(vp);
	fmp = ip->i_mnt;
#ifdef FUSE_DEBUG_VNOP
	printf("read inode=%i, offset=%p, resid=%p\n", ip->i_mnt, 
	       uio->uio_offset, uio->uio_resid);
#endif

	if (uio->uio_resid == 0)
		return (error);
	if (uio->uio_offset < 0)
		return (EINVAL);

	while (uio->uio_resid > 0) {
		bzero(&msg, sizeof(msg));
		msg.hdr = &hdr;
		msg.len = sizeof(fri);
		msg.data = &fri;
		msg.type = msg_buff;
		msg.rep.buff.len = 0;
		msg.rep.buff.data_rcv = NULL;
		msg.cb = &fuse_sync_resp;

		fri.fh = ip->fufh[FUFH_RDONLY].fh_id;
		fri.offset = uio->uio_offset;
		fri.size = MIN(uio->uio_resid, fmp->max_write);

		fuse_make_in(fmp->mp, msg.hdr, msg.len, FUSE_READ, ip->i_number, curproc);

		TAILQ_INSERT_TAIL(&fmq_in, &msg, node);
		wakeup(&fmq_in);

		if ((error = tsleep(&msg, PWAIT, "fuse read", 0)))
			break;

		if ((error = uiomove(msg.rep.buff.data_rcv, MIN(fri.size, msg.rep.buff.len), uio))) {
			break;
		}

		if (msg.rep.buff.len < fri.size)
			break;

		if (msg.rep.buff.data_rcv) {
			free(msg.rep.buff.data_rcv, M_FUSEFS);
			msg.rep.buff.data_rcv = NULL;
		}
	}

	if (msg.rep.buff.data_rcv)
		free(msg.rep.buff.data_rcv, M_FUSEFS);
	return (error);
}

int
fusefs_write(void *v)
{
	struct vop_write_args *ap = v;
	struct vnode *vp = ap->a_vp;
	struct uio *uio = ap->a_uio;
	struct fuse_in_header hdr;
	struct fuse_write_in fwi;
	struct fuse_node *ip;
	struct fuse_mnt *fmp;
	struct fuse_msg msg;
	size_t len;
	int diff;
	int error=0;

#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_write\n");
#endif
	ip = VTOI(vp);
	fmp = ip->i_mnt;
#ifdef FUSE_DEBUG_VNOP
	printf("write inode=%i, offset=%x, resid=%x\n", ip->i_mnt, 
	       uio->uio_offset, uio->uio_resid);
#endif

	if (!uio->uio_resid)
		return (error);

	while (uio->uio_resid > 0) {
		len = MIN(uio->uio_resid, fmp->max_write);
		bzero(&msg, sizeof(msg));
		msg.hdr = &hdr;
		msg.len = sizeof(fwi) + len;
		msg.data = malloc(msg.len, M_FUSEFS, M_WAITOK | M_ZERO);
		msg.type = msg_buff;
		msg.rep.buff.len = sizeof(struct fuse_write_out);
		msg.rep.buff.data_rcv = NULL;
		msg.cb = &fuse_sync_resp;

		fwi.fh = ip->fufh[FUFH_WRONLY].fh_id;
		fwi.offset = uio->uio_offset;
		fwi.size = len;

		memcpy(msg.data, &fwi, sizeof(fwi));
		if ((error = uiomove((char *)msg.data + sizeof(fwi), len, uio)))
			break;

		fuse_make_in(fmp->mp, msg.hdr, msg.len, FUSE_WRITE, ip->i_number, curproc);

		TAILQ_INSERT_TAIL(&fmq_in, &msg, node);
		wakeup(&fmq_in);

		if ((error = tsleep(&msg, PWAIT, "fuse write", 0)))
			break;

		diff = len - ((struct fuse_write_out *)msg.rep.buff.data_rcv)->size;
		if (diff < 0) {
			error = EINVAL;
			break;
		}
		
		uio->uio_resid += diff;
		uio->uio_offset -= diff;

		if (msg.rep.buff.data_rcv) {
			free(msg.rep.buff.data_rcv, M_FUSEFS);
			msg.rep.buff.data_rcv = NULL;
		}
	}

	if (msg.rep.buff.data_rcv)
		free(msg.rep.buff.data_rcv, M_FUSEFS);
	return (error);
}

int
fusefs_poll(void *v)
{
	struct vop_poll_args *ap = v;

#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_poll\n");
#endif

	/*
	 * We should really check to see if I/O is possible.
	 */
	return (ap->a_events & (POLLIN | POLLOUT | POLLRDNORM | POLLWRNORM));
}

int
fusefs_fsync(void *v)
{
#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_fsync\n");
#endif
	return 0;
}

int
fusefs_rename(void *v)
{
#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_rename\n");
#endif
	return 0;
}

int
fusefs_mkdir(void *v)
{
	struct vop_mkdir_args *ap = v;
	struct vnode *dvp = ap->a_dvp;
	struct vnode **vpp = ap->a_vpp;
	struct componentname *cnp = ap->a_cnp;
	struct vattr *vap = ap->a_vap;
	struct fuse_mkdir_in fmdi;
	struct vnode *tdp = NULL;
	struct fuse_in_header hdr;
	struct fuse_entry_out *feo;
	struct fuse_node *ip;
	struct fuse_mnt *fmp;
	struct fuse_msg msg;
	int error = 0;

#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_mkdir %s\n", cnp->cn_nameptr);
#endif
	ip = VTOI(dvp);
	fmp = ip->i_mnt;
	fmdi.mode = MAKEIMODE(vap->va_type, vap->va_mode);

	if (!fmp->sess_init || (fmp->undef_op & UNDEF_MKDIR)) {
		error = ENOSYS;
		goto out;
	}

	bzero(&msg, sizeof(msg));
	msg.hdr = &hdr;
	msg.len = sizeof(fmdi) + cnp->cn_namelen + 1;
	msg.data = malloc(msg.len, M_FUSEFS, M_WAITOK | M_ZERO);
	msg.type = msg_buff;
	msg.rep.buff.len = sizeof(struct fuse_entry_out);
	msg.rep.buff.data_rcv = NULL;
	msg.cb = &fuse_sync_resp;

	memcpy(msg.data, &fmdi, sizeof(fmdi));
	memcpy((char *)msg.data + sizeof(fmdi), cnp->cn_nameptr, cnp->cn_namelen);
	((char *)msg.data)[sizeof(fmdi) + cnp->cn_namelen] = '\0';

	fuse_make_in(fmp->mp, msg.hdr, msg.len, FUSE_MKDIR, ip->i_number, curproc);

	TAILQ_INSERT_TAIL(&fmq_in, &msg, node);
	wakeup(&fmq_in);

	if ((error = tsleep(&msg, PWAIT, "fuse mkdir", 0)))
		goto out;

	if (msg.error) {
		error = msg.error;

		if (error == ENOSYS) {
			fmp->undef_op |= UNDEF_MKDIR;
		}
		goto out;
	}

	feo = (struct fuse_entry_out *)msg.rep.buff.data_rcv;

	if ((error = VFS_VGET(fmp->mp, feo->nodeid, &tdp)))
		goto out;

	tdp->v_type = IFTOVT(feo->attr.mode);
	VTOI(tdp)->vtype = tdp->v_type;

	if (dvp != NULL && dvp->v_type == VDIR)
		VTOI(tdp)->parent = ip->i_number;

	*vpp = tdp;
	VN_KNOTE(ap->a_dvp, NOTE_WRITE | NOTE_LINK);

out:
	vput(dvp);

	if (msg.rep.buff.data_rcv)
		free(msg.rep.buff.data_rcv, M_FUSEFS);

	return (error);
}

int
fusefs_rmdir(void *v)
{
	struct vop_rmdir_args *ap = v;
	struct vnode *vp = ap->a_vp;
	struct vnode *dvp = ap->a_dvp;
	struct componentname *cnp = ap->a_cnp;
	struct fuse_node *ip, *dp;
	struct fuse_in_header hdr;
	struct fuse_mnt *fmp;
	struct fuse_msg msg;
	int error;

#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_rmdir\n");
	printf("len :%i, cnp: %c %c %c\n", cnp->cn_namelen, cnp->cn_nameptr[0], cnp->cn_nameptr[1], cnp->cn_nameptr[2]);
#endif
	ip = VTOI(vp);
	dp = VTOI(dvp);
	fmp = ip->i_mnt;

	/*
	 * No rmdir "." please.
	 */
	if (dp == ip) {
		vrele(dvp);
		vput(vp);
		return (EINVAL);
	}

	if (!fmp->sess_init || (fmp->undef_op & UNDEF_RMDIR)) {
		error = ENOSYS;
		goto out;
	}

	VN_KNOTE(dvp, NOTE_WRITE | NOTE_LINK);

	bzero(&msg, sizeof(msg));
	msg.hdr = &hdr;
	msg.len = cnp->cn_namelen + 1;
	msg.data = malloc(msg.len, M_FUSEFS, M_WAITOK | M_ZERO);
	msg.type = msg_intr;
	msg.cb = &fuse_sync_it;

	memcpy(msg.data, cnp->cn_nameptr, cnp->cn_namelen);
	((char *)msg.data)[cnp->cn_namelen] = '\0';

	fuse_make_in(fmp->mp, msg.hdr, msg.len, FUSE_RMDIR, dp->i_number, curproc);

	TAILQ_INSERT_TAIL(&fmq_in, &msg, node);
	wakeup(&fmq_in);

	if ((error = tsleep(&msg, PWAIT, "fuse rmdir", 0)))
		goto out;

	if (msg.error) {
		error = msg.error;

		if (error == ENOSYS)
			fmp->undef_op |= UNDEF_RMDIR;
		if (error != ENOTEMPTY)
			VN_KNOTE(dvp, NOTE_WRITE | NOTE_LINK);
		goto out;
	}

	VN_KNOTE(dvp, NOTE_WRITE | NOTE_LINK);
	
	cache_purge(dvp);
	vput(dvp);
	dvp = NULL;

	cache_purge(ITOV(ip));
out:
	if (dvp)
		vput(dvp);
	VN_KNOTE(vp, NOTE_DELETE);
	vput(vp);
	return (error);
}

int
fusefs_remove(void *v)
{
	struct vop_remove_args *ap = v;
	struct vnode *vp = ap->a_vp;
	struct vnode *dvp = ap->a_dvp;
	struct componentname *cnp = ap->a_cnp;
	struct fuse_node *ip;
	struct fuse_node *dp;
	struct fuse_in_header hdr;
	struct fuse_mnt *fmp;
	struct fuse_msg msg;
	int error = 0;

#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_remove\n");
	printf("len :%i, cnp: %c %c %c\n", cnp->cn_namelen, cnp->cn_nameptr[0], cnp->cn_nameptr[1], cnp->cn_nameptr[2]);
#endif
	ip = VTOI(vp);
	dp = VTOI(dvp);
	fmp = ip->i_mnt;

	if (!fmp->sess_init || (fmp->undef_op & UNDEF_REMOVE)) {
		error = ENOSYS;
		goto out;
	}

	bzero(&msg, sizeof(msg));
	msg.hdr = &hdr;
	msg.len = cnp->cn_namelen + 1;
	msg.data = malloc(msg.len, M_FUSEFS, M_WAITOK | M_ZERO);
	msg.type = msg_intr;
	msg.cb = &fuse_sync_it;

	memcpy(msg.data, cnp->cn_nameptr, cnp->cn_namelen);
	((char *)msg.data)[cnp->cn_namelen] = '\0';

	fuse_make_in(fmp->mp, msg.hdr, msg.len, FUSE_UNLINK, dp->i_number, curproc);

	TAILQ_INSERT_TAIL(&fmq_in, &msg, node);
	wakeup(&fmq_in);

	
	if ((error = tsleep(&msg, PWAIT, "fuse remove", 0)))
		goto out;

	if (msg.error) {
		error = msg.error;

		if (error == ENOSYS)
			fmp->undef_op |= UNDEF_REMOVE;
		goto out;
	}

	
	VN_KNOTE(vp, NOTE_DELETE);
	VN_KNOTE(dvp, NOTE_WRITE);
out:
	if (dvp == vp)
		vrele(vp);
	else
		vput(vp);
	vput(dvp);
	return (error);
}

int
fusefs_strategy(void *v)
{
#ifdef FUSE_DEBUG_VNOP
	printf("fusefs_strategy\n");
#endif
	return 0;
}

int
fusefs_lock(void *v)
{
	struct vop_lock_args *ap = v;
	struct vnode *vp = ap->a_vp;

#ifdef FUSE_LOCK
	printf("fuse_lock\n");
#endif
	return (lockmgr(&VTOI(vp)->i_lock, ap->a_flags, NULL));  
}

int
fusefs_unlock(void *v)
{
	struct vop_unlock_args *ap = v;
	struct vnode *vp = ap->a_vp;

#ifdef FUSE_LOCK
	printf("fuse_unlock\n");
#endif
	return (lockmgr(&VTOI(vp)->i_lock, ap->a_flags | LK_RELEASE, NULL));
}

int
fusefs_islocked(void *v)
{
	struct vop_islocked_args *ap = v;

#ifdef FUSE_LOCK
	printf("fuse_islock\n");
#endif
	return (lockstatus(&VTOI(ap->a_vp)->i_lock));
}

int
fusefs_advlock(void *v)
{
	struct vop_advlock_args *ap = v;
	struct fuse_node *ip = VTOI(ap->a_vp);

#ifdef FUSE_LOCK
	printf("fuse_advlock\n");
#endif
	return (lf_advlock(&ip->i_lockf, ip->filesize, ap->a_id, ap->a_op,
	    ap->a_fl, ap->a_flags));
}
