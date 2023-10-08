#testing that the help option lists every tag
output=$(./extfat -h 2>&1)
for tag in "h" "i" "o" "c" "v" "x" "D"
    do
    if [[ "$output" != *"-$tag"* ]]; #testing that the -h option explains every option listed in requirements
    then
        exit 1
    fi
done