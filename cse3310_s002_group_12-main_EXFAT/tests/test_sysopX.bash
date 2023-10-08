#testing the -c option of extfat utility


echo "Running... " > textfile.txt 
cp textfile.txt /tmp/d
sync
sleep .5


hash1=($(/usr/bin/md5sum textfile.txt))
output=$(./extfat -i test.image -o out.txt -x textfile.txt 2>&1)
sync
sleep .5
fileexist="$(ls /workspaces/cse3310_s002_group_12)"
if [[ "$fileexist" != *"out.txt"* ]];
then
    exit 1
fi
    
hash2=($(/usr/bin/md5sum out.txt))

(rm out.txt 2>&1 ) > /dev/null
(rm /tmp/d/textfile.txt 2>&1) > /dev/null
(rm textfile.txt 2>&1) > /dev/null

#echo $hash1
#echo $hash2


if [[ "$hash1" != "$hash2" ]];
#we exit 0 1 on fail
then

    exit 1
fi