#making sure that the system doesnt accept the -c and -x options together
echo "testFile" > /tmp/d/test.txt
sync
output=$(./extfat -i test.image -x a/b -c -o out.txt 2>&1)
if [[ $output != *"incompatible"* ]];
then
    exit 1
fi
