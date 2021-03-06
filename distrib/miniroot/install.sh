#!/bin/ksh
#	$OpenBSD: install.sh,v 1.233 2013/05/31 06:27:08 rpe Exp $
#	$NetBSD: install.sh,v 1.5.2.8 1996/08/27 18:15:05 gwr Exp $
#
# Copyright (c) 1997-2009 Todd Miller, Theo de Raadt, Ken Westerback
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Copyright (c) 1996 The NetBSD Foundation, Inc.
# All rights reserved.
#
# This code is derived from software contributed to The NetBSD Foundation
# by Jason R. Thorpe.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
# ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

#	Bitrig installation script.
#	In a perfect world, this would be a nice C program, with a reasonable
#	user interface.

# install.sub needs to know the MODE
MODE=install

# include common subroutines and initialization code
. /install.sub

DISK=
DISKS_DONE=
_DKDEVS=$(get_dkdevs)
_fsent=

# Remove traces of previous install attempt.
rm -f /tmp/fstab.shadow /tmp/fstab /tmp/fstab.*

ask_yn "Use DUIDs rather than device names in fstab?" yes
[[ $resp == y ]] && FSTABFLAG=-F

while :; do
	DISKS_DONE=$(addel "$DISK" $DISKS_DONE)
	_DKDEVS=$(rmel "$DISK" $_DKDEVS)

	# Always do ROOTDISK first, and repeat until it is configured.
	if isin $ROOTDISK $_DKDEVS; then
		resp=$ROOTDISK
		rm -f /tmp/fstab
	else
		# Force the user to think and type in a disk name by
		# making 'done' the default choice.
		ask_which "disk" "do you wish to initialize" \
			'$(l=$(get_dkdevs); for a in $DISKS_DONE; do
				l=$(rmel $a $l); done; bsort $l)' \
			done
		[[ $resp == done ]] && break
	fi

	DISK=$resp
	makedev $DISK || continue

	# Deal with disklabels, including editing the root disklabel
	# and labeling additional disks. This is machine-dependent since
	# some platforms may not be able to provide this functionality.
	# /tmp/fstab.$DISK is created here with 'disklabel -f' or
	# 'disklabel -F' depending on the value of $FSTABFLAG.
	rm -f /tmp/*.$DISK
	AUTOROOT=n
	md_prep_disklabel $DISK || { DISK= ; continue ; }

	# Make sure there is a '/' mount point.
	grep -qs " / ffs " /tmp/fstab.$ROOTDISK ||
		{ DISK= ; echo "'/' must be configured!" ; continue ; }

	if [[ -f /tmp/fstab.$DISK ]]; then
		# Avoid duplicate mount points on different disks.
		while read _pp _mp _rest; do
			if [[ $_mp == none ]]; then
				# Multiple swap partitions are ok.
				echo "$_pp $_mp $_rest" >>/tmp/fstab
				continue
			fi
			# Non-swap mountpoints must be in only one file.
			[[ /tmp/fstab.$DISK == $(grep -l " $_mp " /tmp/fstab.*) ]] ||
				{ _rest=$DISK ; DISK= ; break ; }
		done </tmp/fstab.$DISK
		if [[ -z $DISK ]]; then
			# Duplicate mountpoint.
			# Allow disklabel(8) to read back mountpoint info
			# if it is immediately run against the same disk.
			cat /tmp/fstab.$_rest >/etc/fstab
			rm /tmp/fstab.$_rest
			set -- $(grep -h " $_mp " /tmp/fstab.*[0-9])
			echo "$_pp and $1 can't both be mounted at $_mp."
			continue
		fi

		# Add ffs filesystems to list after newfs'ing them. Ignore
		# other filesystems.
		while read _pp _mp _fstype _rest; do
			[[ $_fstype == ffs ]] || continue
			_OPT=
			[[ $_mp == / ]] && _OPT=$MDROOTFSOPT
			newfs -q $_OPT ${_pp##/dev/}
			# N.B.: '!' is lexically < '/'. That is
			#	required for correct sorting of
			#	mount points.
			_fsent="$_fsent $_mp!$_pp"
		done </tmp/fstab.$DISK
	fi
done

# Write fstab entries to fstab in mount point alphabetic order
# to enforce a rational mount order.
for _mp in $(bsort $_fsent); do
	_pp=${_mp##*!}
	_mp=${_mp%!*}
	echo -n "$_pp $_mp ffs rw"

	# Only '/' is neither nodev nor nosuid. i.e. it can obviously
	# *always* contain devices or setuid programs.
	[[ $_mp == / ]] && { echo " 1 1" ; continue ; }

	# Every other mounted filesystem is nodev. If the user chooses
	# to mount /dev as a separate filesystem, then on the user's
	# head be it.
	echo -n ",nodev"

	# The only directories that the install puts suid binaries into
	# (as of 3.2) are:
	#
	# /sbin
	# /usr/bin
	# /usr/sbin
	# /usr/libexec
	# /usr/libexec/auth
	# /usr/X11R6/bin
	#
	# and ports and users can do who knows what to /usr/local and
	# sub directories thereof.
	#
	# So try to ensure that only filesystems that are mounted at
	# or above these directories can contain suid programs. In the
	# case of /usr/libexec, give blanket permission for
	# subdirectories.
	case $_mp in
	/sbin|/usr)			;;
	/usr/bin|/usr/sbin)		;;
	/usr/libexec|/usr/libexec/*)	;;
	/usr/local|/usr/local/*)	;;
	/usr/X11R6|/usr/X11R6/bin)	;;
	*)	echo -n ",nosuid"	;;
	esac
	echo " 1 2"
done >>/tmp/fstab

munge_fstab
mount_fs "-o async"

install_sets

# If we did not succeed at setting TZ yet, we try again
# using the timezone names extracted from the base set
if [[ -z $TZ ]]; then
	(cd /mnt/usr/share/zoneinfo
	    ls -1dF $(tar cvf /dev/null [A-Za-y]*) >/mnt/tmp/tzlist )
	echo
	set_timezone /mnt/tmp/tzlist
	rm -f /mnt/tmp/tzlist
fi

# Ensure an enabled console has the correct speed in /etc/ttys.
sed -e "/^console.*on.*secure.*$/s/std\.[0-9]*/std.$(stty speed)/" \
	/mnt/etc/ttys >/tmp/ttys
mv /tmp/ttys /mnt/etc/ttys

echo -n "Saving configuration files..."

# Save any leases obtained during install.
(cd /var/db; [[ -f dhclient.leases ]] && mv dhclient.leases /mnt/var/db/. )

# Move configuration files from /tmp to /mnt/etc.
hostname >/tmp/myname

# Append entries to installed hosts file, changing '1.2.3.4 hostname'
# to '1.2.3.4 hostname.$FQDN hostname'. Leave untouched lines containing
# domain information or aliases. These are lines the user added/changed
# manually. Note we may have no hosts file if no interfaces were configured.
if [[ -f /tmp/hosts ]]; then
	_dn=$(get_fqdn)
	while read _addr _hn _aliases; do
		if [[ -n $_aliases || $_hn != ${_hn%%.*} || -z $_dn ]]; then
			echo "$_addr\t$_hn $_aliases"
		else
			echo "$_addr\t$_hn.$_dn $_hn"
		fi
	done </tmp/hosts >>/mnt/etc/hosts
	rm /tmp/hosts
fi

# Append dhclient.conf to installed dhclient.conf.
_f=dhclient.conf
[[ -f /tmp/$_f ]] && { cat /tmp/$_f >>/mnt/etc/$_f ; rm /tmp/$_f ; }

# Possible files to copy from /tmp: fstab hostname.* kbdtype mygate
#     myname ttys boot.conf resolv.conf sysctl.conf resolv.conf.tail
# Save only non-empty (-s) regular (-f) files.
(cd /tmp; for _f in fstab hostname* kbdtype my* ttys *.conf *.tail; do
	[[ -f $_f && -s $_f ]] && mv $_f /mnt/etc/.
done)

# Feed the random pool some junk before we read from it
(dmesg; cat $SERVERLISTALL; sysctl; route -n show; df;
    ifconfig -A; hostname) >/mnt/dev/arandom 2>&1

echo -n "done.\nGenerating initial host.random file..."
dd if=/mnt/dev/arandom of=/mnt/var/db/host.random \
	bs=65536 count=1 >/dev/null 2>&1
chmod 600 /mnt/var/db/host.random >/dev/null 2>&1
echo "done."

apply

if [[ -n $user ]]; then
	_encr="*"
	[[ -n "$userpass" ]] && _encr=$(/mnt/usr/bin/encrypt -b 8 -- "$userpass")
	uline="${user}:${_encr}:1000:1000:staff:0:0:${username}:/home/${user}:/bin/ksh"
	echo "$uline" >> /mnt/etc/master.passwd
	echo "${user}:*:1000:" >> /mnt/etc/group
	echo ${user} > /mnt/root/.forward

	mkdir -p /mnt/home/$user
	(cd /mnt/etc/skel; cp -pR . /mnt/home/$user)
	chown -R 1000:1000 /mnt/home/$user
	echo "1,s@wheel:.:0:root\$@wheel:\*:0:root,${user}@
w
q" | ed /mnt/etc/group 2>/dev/null
fi

if [[ -n "$_rootpass" ]]; then
	_encr=$(/mnt/usr/bin/encrypt -b 8 -- "$_rootpass")
	echo "1,s@^root::@root:${_encr}:@
w
q" | ed /mnt/etc/master.passwd 2>/dev/null
fi
/mnt/usr/sbin/pwd_mkdb -p -d /mnt/etc /etc/master.passwd

if grep -qs '^rtsol' /mnt/etc/hostname.*; then
	sed -e "/^#\(net\.inet6\.ip6\.accept_rtadv\)/s//\1/" \
	    -e "/^#\(net\.inet6\.icmp6\.rediraccept\)/s//\1/" \
		/mnt/etc/sysctl.conf >/tmp/sysctl.conf
	cp /tmp/sysctl.conf /mnt/etc/sysctl.conf
fi

# Perform final steps common to both an install and an upgrade.
finish_up
