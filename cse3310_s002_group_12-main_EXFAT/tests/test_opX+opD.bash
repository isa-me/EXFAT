#ensure that the system doesn't accept -x and -D
echo "testFile" > /tmp/d/test.txt
sync
output=$(./extfat -i test.image -x a/b -D a/b -o out.txt 2>&1)
if [[ $output != *"incompatible"* ]];
then
    exit 1
fi

