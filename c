#!/bin/sh
fname="$HOME/a5/cs488_hosts"

export RUN_TYPE=COORDINATOR
cd ~/a5
make

exec<$fname
numLines=0

while read line
do
    ssh -n cm2chou@$line "nohup pkill rt -u cm2chou &"
    ssh -n cm2chou@$line "nohup a5/w > & ~/test1 &"
done

./rt