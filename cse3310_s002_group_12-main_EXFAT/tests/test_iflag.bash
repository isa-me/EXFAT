#ensuring that the system requires the -i option
#this should be handled in processArgs.c
output=$(./extfat 2>&1)
if [[ "$output" != *"Specify an input file with the -i option"* ]];
then
    exit 1
fi





