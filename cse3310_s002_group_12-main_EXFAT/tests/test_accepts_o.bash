output=$(./extfat -i test.image -o out.image)
if [[ "$output" != *"file output: out.image"* ]]
then
    exit 1
fi