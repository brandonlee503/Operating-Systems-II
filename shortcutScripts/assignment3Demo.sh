#!/bin/bash

# Assignment 3 Demo Assigment 3 from existing Linux 
# Source file - Usage: ". assignment3Demo.sh"

source /scratch/opt/environment-setup-i586-poky-linux

# Go to directory
cd /scratch/spring2016/cs444-074/linux-yocto-3.14/

# Make clean
make clean

# Assigment 3 Stuff
echo "Assignment 3 - Setting config files"

#TODO: Update to take from patchfile
yes | cp -rf /scratch/spring2016/cs444-074/projects/project3/backupfiles/* /scratch/spring2016/cs444-074/linux-yocto-3.14/drivers/block/

# Build everything
printf "Building fresh VM/kernel...\n"
make -j4 all

# And finally run
printf "Running QEMU command...\n"
qemu-system-i386 -gdb tcp::5574 -S -nographic -kernel arch/x86/boot/bzImage -drive file=core-image-lsb-sdk-qemux86.ext3,if=ide -enable-kvm -usb -localtime --no-reboot --append "root=/dev/hda rw console=ttyS0 debug"
