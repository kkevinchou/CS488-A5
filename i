#!/bin/sh
fname="$HOME/a5/cs488_hosts"

exec<$fname

while read line
do
    hostLength=${#line}

    if [ $hostLength -eq 4 ]
    then
        ssh -n cm2chou@$line mkdir -p .ssh
        cat ~/.ssh/id_rsa.pub | ssh -n cm2chou@$line 'cat >> .ssh/authorized_keys'
    fi
done