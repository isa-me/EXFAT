#these will throw actual errors
#these are handled in fileMapping.c
output=$(./extfat -i doesntexist.image -o out.image -c 2>&1)
if [[ "$output" != *"No such file or directory"* ]];
then
    exit 1
fi


output=$(./extfat -i doesntexist.image -v 2>&1)
if [[ "$output" != *"No such file or directory"* ]];
then
    exit 1
fi
