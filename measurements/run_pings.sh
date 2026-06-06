#!/bin/bash

file="hosts.txt"
while read -r line
do
    ping -c5 "$line"
done < "$file"

