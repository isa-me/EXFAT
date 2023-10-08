#/bin/bash
# this script is designed to make images to assist in developing and
# testing the program(s)
dd if=/dev/zero of=test.image count=1 bs=100M
sudo losetup /dev/loop2 test.image
sudo /usr/sbin/mkexfatfs /dev/loop2
#
#
#
# 
mkdir /tmp/d
sudo mount /dev/loop2 /tmp/d
cp abc /tmp/d -r
cp src/ /tmp/d -r
cp tests/ /tmp/d -r
cp unit_tests /tmp/d -r
cp common /tmp/d -r