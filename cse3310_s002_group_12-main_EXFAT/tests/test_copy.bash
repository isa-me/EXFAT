#testing the -c option of extfat utility


hash1=($(/usr/bin/md5sum test.image))
output=$(./extfat -i test.image -c -o out.image)
hash2=($(/usr/bin/md5sum out.image))

rm out.image
#echo $hash1
#echo $hash2

if [[ "$hash1" != "$hash2" ]];
#we exit 0 1 on fail
then
    exit 1
fi