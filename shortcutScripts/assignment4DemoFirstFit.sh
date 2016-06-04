#!/bin/bash

# Source file - Usage: ". assignment4DemoFirstFit.sh"
source /scratch/opt/environment-setup-i586-poky-linux

# Go to directory
cd /scratch/spring2016/cs444-074/linux-yocto-3.14/

# Make clean
make clean

# Assigment 4 Stuff
echo "Assignment 4 - Setting slob files"
yes | cp -rf /scratch/spring2016/cs444-074/projects/project4/backupfiles/slobNewOld.c /scratch/spring2016/cs444-074/linux-yocto-3.14/mm/
yes | mv /scratch/spring2016/cs444-074/linux-yocto-3.14/mm/slobOldNew.c /scratch/spring2016/cs444-074/linux-yocto-3.14/mm/slob.c
make -j4 all

# And finally run
printf "Running QEMU command...\n"
qemu-system-i386 -gdb tcp::5574 -S -nographic -kernel arch/x86/boot/bzImage -drive file=core-image-lsb-sdk-qemux86.ext3,if=ide -enable-kvm -usb -localtime --no-reboot --append "root=/dev/hda rw console=ttyS0 debug"
