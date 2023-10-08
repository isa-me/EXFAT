# ignore for now


#cp -r tests /tmp/d

mkdir /tmp/d/a
mkdir /tmp/d/a/b
mkdir /tmp/d/a/b/c
touch /tmp/d/a/b/long_________________________________________________________________________filename.txt
sync
sleep .5 #doing this because sync doesnt stop all processes
output=$(./extfat -i test.image -d)
for word in 'a' 'b' 'c' 'long_________________________________________________________________________filename.txt'
do
    if [[ "$output" != *$word* ]]
    then
        #echo failed on $word
        exit 1
    fi
done
if [[ "$output" != *"a>>>"*"b>>>"*"c>>>"* ]]
then
    exit 1
fi