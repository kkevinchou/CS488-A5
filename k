#!/bin/sh
fname="$HOME/a5/cs488_hosts"

exec<$fname

while read line
do
    hostLength=${#line}

    if [ $hostLength -eq 4 ]
    then
        ssh -n cm2chou@$line "nohup pkill rt -u cm2chou &"
    fi
done