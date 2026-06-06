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
    traceroute -I -A -n "$line"
done < "$file"
