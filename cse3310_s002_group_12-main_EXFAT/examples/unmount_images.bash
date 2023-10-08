#!/bin/bash
# This script will unmount an image
sudo umount /tmp/d
sudo losetup -d /dev/loop2
rm -rf /tmp/d
rm test.image