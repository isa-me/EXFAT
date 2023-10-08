echo "Running System Tests"


passed=0
failed=0
#all these scripts will be run once for each size
for script in $(ls tests/test_*.bash)

do
    for sectorSize in '1' '4' '8'
    do
        for size in '100K' '500K' '1M' #'5M' '50M' #running all this code for 5 different image sizes
            do
            dd if=/dev/zero of=test.image count=1 bs=$size status=none > /dev/null #making a file of just zeroes
            sudo losetup /dev/loop2 test.image > /dev/null #setting up our loop device
            sudo /usr/sbin/mkexfatfs -s $sectorSize /dev/loop2 > /dev/null #making our extfat file image
            mkdir /tmp/d
            sudo mount /dev/loop2 /tmp/d > /dev/null
            bash $script $size
            if [ $? = 0 ];
            then
                echo "$script $size sectorSize $sectorSize: PASSED"
                passed=$((passed+1))
            else
                echo "$script $size sectorSize $sectorSize: FAILED"
                failed=$((failed+1))
            fi
            bash tests/clean_up.bash
        done
    done
done



echo "$passed tests passed"
echo "$failed tests failed"