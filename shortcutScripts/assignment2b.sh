#!/bin/bash

# Assignment 2b - Build Assignment 2 from already existing assignment 1
# Source file - Usage: ". assignment2b.sh"

source /scratch/opt/environment-setup-i586-poky-linux

# Go to directory
cd /scratch/spring2016/cs444-074/

# Note this command may vary depending on your path to your files 
printf "Updating scheduler files\n"
yes | cp -rf /scratch/spring2016/cs444-074/projects/project1/backupfiles/* /scratch/spring2016/cs444-074/linux-yocto-3.14/block/

# Set default scheduler
printf "Now need to manually set your default scheduler in 'make menuconfig'.\n"
make menuconfig

# Build everything
printf "Building fresh VM/kernel...\n"
make -j4 all

# And finally run
printf "Running QEMU command...\n"
qemu-system-i386 -gdb tcp::5574 -S -nographic -kernel arch/x86/boot/bzImage -drive file=core-image-lsb-sdk-qemux86.ext3,if=ide -enable-kvm -net none -usb -localtime --no-reboot --append "root=/dev/hda rw console=ttyS0 debug"
