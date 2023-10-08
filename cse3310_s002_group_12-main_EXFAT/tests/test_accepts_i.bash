output=$(./extfat -i test.image 2>&1) 
if [[ "$output" != *"file input: test.image"* ]]
then
    exit 1
fi