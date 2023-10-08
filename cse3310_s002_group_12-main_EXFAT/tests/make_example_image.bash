dd if=/dev/zero of=test.image count=1 bs=100M status=none  #making a file of just zeroes
        sudo losetup /dev/loop2 test.image  #setting up our loop device
        sudo /usr/sbin/mkexfatfs -s 4 /dev/loop2  #making our extfat file image
        mkdir /tmp/d
        sudo mount /dev/loop2 /tmp/d
        cp tests /tmp/d -r
        cp Makefile /tmp/d -r
        mkdir /tmp/d/a
        mkdir /tmp/d/a/b
        mkdir /tmp/d/a/b/c
        touch /tmp/d/a/b/file.txt
        sync
        sleep 1
