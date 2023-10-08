echo "testFile" > /tmp/d/test.txt
sync
output=$(./extfat -i test.image -x test/txt 2>&1)
if [[ $output != *"Enter an output file"* ]];
then
    exit 1
fi
