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

tmpdir=$(mktemp -d)

tmpdir=$(mktemp -d)
mkdir -p "$tmpdir/boot/grub"
cp -r "$archdir/grub2/grub.cfg" "$tmpdir/boot/grub/"
cp "$kernel" "$tmpdir/boot/"
cp "$initrd" "$tmpdir/boot/"

grub2-mkrescue -o boot.iso "$tmpdir"
rm -rf "$tmpdir"
