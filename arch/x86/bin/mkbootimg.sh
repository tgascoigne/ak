#!/bin/sh

set -ex

if [ $# -ne 3 ]; then
    echo "Usage: $0 <output> <kernel> <initrd>"
    exit 1
fi

archdir=$(dirname "$(readlink -f "$0")")/..

bootimg=$1
kernel=$2
initrd=$3

tmpmount=$(mktemp -d)

loopdev0=/dev/loop0
loopdev1=/dev/loop1

# Create the partitioned image
dd if=/dev/zero of=$bootimg bs=512 count=131072
sfdisk $bootimg < "$archdir/boot.sfdisk"

cleanup=""

# Mount it
losetup $loopdev0 $bootimg
cleanup="$cleanup losetup -d $loopdev0 ; "
trap "$cleanup" INT TERM EXIT

losetup $loopdev1 $bootimg -o 1048576
cleanup="$cleanup losetup -d $loopdev1 ; "
trap "$cleanup" INT TERM EXIT

# Format it
mkfs.msdos $loopdev1

# Mount it
mount $loopdev1 $tmpmount
cleanup="$cleanup umount $tmpmount ; "
trap "$cleanup" INT TERM EXIT

# Install grub
grub2-install --root-directory="$tmpmount" --no-floppy --modules "normal part_msdos ext2 multiboot" $loopdev0

# Install the kernel and ramdisk
cp -r "$archdir/grub2" "$tmpmount/boot/"
cp "$kernel" "$tmpmount/boot/"
cp "$initrd" "$tmpmount/boot/"
