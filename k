#!/bin/sh
fname="$HOME/a5/cs488_hosts"

exec<$fname
numLines=0

while read line
do
    ssh -n cm2chou@$line "nohup pkill rt -u cm2chou &"
done