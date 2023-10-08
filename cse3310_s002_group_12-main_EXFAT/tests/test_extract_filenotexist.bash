output=$(./extfat -i test.image -x this/shouldnt/exist.txt -o out 2>&1)
if [[ $output != *"File Not Found"* ]];
then
    exit 1
fi
