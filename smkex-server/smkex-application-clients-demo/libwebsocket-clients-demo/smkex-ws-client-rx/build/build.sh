#!/bin/bash


# Delete everything except the current script
ls | grep -v 'build.sh' | xargs rm -rf

# Make project
cmake .. 
make -j8

# Run 
./smkex-ws-client-rx
