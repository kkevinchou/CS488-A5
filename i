#!/bin/sh
fname="$HOME/a5/cs488_hosts"

exec<$fname
numLines=0

while read line
do
    ssh -n cm2chou@$line mkdir -p .ssh
    cat ~/.ssh/id_rsa.pub | ssh -n cm2chou@$line 'cat >> .ssh/authorized_keys'
done