#!/bin/bash

# Assignment 2 Fully create from scratch with patch files. 
# Source file - Usage: ". assignment2patch.sh"

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

# Assignment 2 Stuff
# Note this command may vary depending on your path to your files 
printf "Updating scheduler files\n"
cd ../
patch < /scratch/spring2016/cs444-074/projects/project2/patch/project2.patch 
mv -f Kconfig.iosched Makefile sstf-iosched.c linux-yocto-3.14/block/
cd linux-yocto-3.14/

# Set default scheduler
printf "Now need to manually set your default scheduler in 'make menuconfig'.\n"
make menuconfig

# Build everything
printf "Building fresh VM/kernel...\n"
make -j4 all

# And finally run
printf "Running QEMU command...\n"
qemu-system-i386 -gdb tcp::5574 -S -nographic -kernel arch/x86/boot/bzImage -drive file=core-image-lsb-sdk-qemux86.ext3,if=ide -enable-kvm -net none -usb -localtime --no-reboot --append "root=/dev/hda rw console=ttyS0 debug"
