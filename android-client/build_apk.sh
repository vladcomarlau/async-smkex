#!/bin/bash

# Sanity checks
if [[ $# -eq 1 ]] ; then
	# Perform make clean
    if [[ $1 = "-c" ]] ; then
		./clean.sh
	fi
fi

# Environment config check
if [[ -z $ANDROID_NDK_ROOT ]]; then
	echo "Please configure ANDROID_NDK_ROOT environment variable to point to the Android NDK directory (e.g. export ANDROID_NDK_ROOT=/opt/android-ndk-r12b)"
	exit 1
fi

# Build apk file 
echo -e "\n\nStart APK assembling process"
./gradlew assembleDebug


# Display apk location
echo -e "\n\nThe APK(s) are available here:"
find . -name *.apk

# Obtain the last generated apk
apkPath="`find . -name *.apk | head -n1`"

# Check if the apk was generated
if [ -z "$apkPath" ]; then
	echo "No generated apk found"
	exit 1
fi

# Prompt to install apk
echo -e "\n\nDo you want to install the apk? [y/n]"
read apkInstall

if [[ $apkInstall = "n" ]]; then
	exit 0;

else
	# Display available devices
	echo -e "\n\nAvailable Android devices:"
	adb devices

	echo -e "\n\nEnter mobile device ID: "
	read deviceId

	echo -e "\n\nInstalling apk $apkPath on $deviceId"
	adb -s $deviceId install -r $apkPath

	exit 0;
fi



