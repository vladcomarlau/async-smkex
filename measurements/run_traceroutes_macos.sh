#!/bin/bash

if [ $# -eq 0 ]
then
    file="hosts.txt"
else
    file=$1
fi

while read -r line
do
    echo "Doing $line"
    traceroute -P ICMP -z 100 -m 30 -w 1 -e -f 3 -a -n "$line"
done < "$file"
