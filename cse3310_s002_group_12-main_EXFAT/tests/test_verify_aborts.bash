#creating a new file
fileSize=$(stat --format=%s "test.image")

#testing if verify detects a difference when we change the FileSystemName to "invalid "
echo "   invalid " > test2.image #echoing 11 bytes into a new image (jumpBoot and fileName)
bs=512

dd if=test.image of=test2.image bs=1 count=$(($bs - 11)) skip=11 seek=11 status=none > /dev/null #copying until we get a file of size 512
dd if=test.image of=test2.image count=$((($fileSize - $bs)/$bs)) skip=1 seek=1 bs=$bs status=none > /dev/null   #copying the rest of the image 512 bytes at a time





output=$(./extfat -i test2.image -v -c -o shouldntexist.image 2>&1)
rm test2.image

lsoutput=$(ls)
if [[ $lsoutput == *"shouldntexist.image"* ]];
then
    echo $lsoutput
    exit 1
fi