lsoutput1=$(ls)
output=$(./extfat -i test.image -c 2>&1)
if [[ $output != *"Specify output file"* ]];
then
    exit 1
fi
lsoutput2=$(ls)
if [[ $lsoutput1 != $lsoutput2 ]]; #the output of ls changes if extfat created a file
then
    exit 1
fi
