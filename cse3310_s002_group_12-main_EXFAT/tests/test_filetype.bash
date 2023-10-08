#testing that the extfat program only accepts .image files


output=$(./extfat -i badarg 2>&1)
if [[ "$output" != *"Enter a .image file for input"* ]];
then
    exit 1
fi



