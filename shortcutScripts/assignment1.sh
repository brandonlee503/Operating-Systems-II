#!/bin/bash


# Assignment 1 - Build kernel
# Source file - Usage: ". assignment1.sh"

printf "Building fresh VM/kernel...\n"
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

# Build
make -j4 all

# And finally run
printf "Running QEMU command...\n"

# Project 1
#qemu-system-i386 -gdb tcp::5574 -S -nographic -kernel bzImage-qemux86.bin -drive file=core-image-lsb-sdk-qemux86.ext3,if=virtio -enable-kvm -net none -usb -localtime --no-reboot --append "root=/dev/vda rw console=ttyS0 debug"

# Project 2
#qemu-system-i386 -gdb tcp::5574 -S -nographic -kernel arch/x86/boot/bzImage -drive file=core-image-lsb-sdk-qemux86.ext3,if=ide -enable-kvm -net none -usb -localtime --no-reboot --append "root=/dev/hda rw console=ttyS0 debug"

# Email version
qemu-system-i386 -gdb tcp::5574 -S -nographic -kernel arch/x86/boot/bzImage -drive file=core-image-lsb-sdk-qemux86.ext3,if=virtio -enable-kvm -net none -usb -localtime --no-reboot --append "root=/dev/vda rw console=ttyS0 debug"
