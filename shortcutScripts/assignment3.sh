#!/bin/bash

# Assignment 3 Fully create from scratch. 
# Source file - Usage: ". assignment3.sh"

source /scratch/opt/environment-setup-i586-poky-linux

# Go to directory
cd /scratch/spring2016/cs444-074/

# Clone and checkout
git clone git://git.yoctoproject.org/linux-yocto-3.14
cd linux-yocto-3.14/
git checkout v3.14.26

# Copy in required files for QEMU command
cp /scratch/spring2016/files/bzImage-qemux86.bin /scratch/spring2016/files/core-image-lsb-sdk-qemux86.ext3 /scratch/spring2016/cs444-074/linux-yocto-3.14/

# Copy over .config file
cp /scratch/spring2016/files/config-3.14.26-yocto-qemu /scratch/spring2016/cs444-074/linux-yocto-3.14/.config

# Build everything
printf "Building fresh VM/kernel...\n"
make -j4 all

# Assigment 3 Stuff
echo "Assignment 3 - Setting config files"
yes | cp -rf /scratch/spring2016/cs444-074/projects/project3/backupfiles/* /scratch/spring2016/cs444-074/linux-yocto-3.14/drivers/block/
make -j4 all

# And finally run
printf "Running QEMU command...\n"
qemu-system-i386 -gdb tcp::5574 -S -nographic -kernel arch/x86/boot/bzImage -drive file=core-image-lsb-sdk-qemux86.ext3,if=ide -enable-kvm -usb -localtime --no-reboot --append "root=/dev/hda rw console=ttyS0 debug"
