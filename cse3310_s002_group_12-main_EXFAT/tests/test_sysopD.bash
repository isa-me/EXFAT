echo "Running... " > textfile.txt 

cp textfile.txt /tmp/d
sync
sudo umount -q /tmp/d

output=$(./extfat -D textfile.txt -i test.image  2>&1)




(sudo mount /dev/loop2 /tmp/d 2>&1) > /dev/null

sync
sleep .5

fileexist=$(ls /tmp/d) 

if [[ "$fileexist" == *"textfile.txt"* ]];
then
    exit 1
fi