sudo umount -q /tmp/d       #unmounting the image we created
sudo losetup -d /dev/loop2  #removing loop device (i think?)
rm -rf /tmp/d               #remove temp directory 
rm test.image               #remove test image