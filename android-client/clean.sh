#!/bin/bash

CRT_PATH=`pwd`
JNI_PATH="$CRT_PATH/app/src/main/jni/"
BUILD_PATH="$CRT_PATH/app"

# Clean JNI libs 
echo -e "\n\nPerform make clean"

cd $JNI_PATH
make -f NativeLibs.mk clean-ndk
make -f NativeLibs.mk clean
make -f NativeLibs.mk clean-sources
make -f NativeLibs.mk clean-toolchains
make -f NativeLibs.mk clean-targets

# Clean obj
echo -e "\n\nDelete obj files"
cd $BUILD_PATH
rm -rf build