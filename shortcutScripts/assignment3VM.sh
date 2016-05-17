#!/bin/bash

# Assignment 3 VM Portion

# Bring over module (.ko file)
echo "Bringing over module"
scp leebran@os-class.engr.oregonstate.edu:/scratch/spring2016/cs444-074/linux-yocto-3.14/drivers/block/sbd.ko /home/root

# Install module with parameters
echo "Installing module"
insmod sbd.ko key="1234567890123456" keylen=16

# Create partition
echo "Creating partition"
fdisk /dev/sbd0

# Create filesystem on top
echo "Building filesystem"
mkfs.ext2 /dev/sbd0p1

# Mount file system
echo "Mounting file system"
mount /dev/sbd0p1 /testing
