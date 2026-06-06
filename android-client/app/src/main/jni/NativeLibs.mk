SHELL := /bin/bash

ifeq ($(MAKE_NO_INCLUDES),)
-include Application.mk
endif

# Select the ABIs to compile for
#
NDK_APP_ABI = $(APP_ABI)
ifeq ($(NDK_APP_ABI),)
# Set to 'all' if APP_ABI is undefined
NDK_APP_ABI = all
endif
ifeq ($(NDK_APP_ABI),all)
# Translate 'all' to the individual targets
NDK_APP_ABI = armeabi armeabi-v7a arm64-v8a
else
# Use the targets from APP_ABI
NDK_APP_ABI = $(APP_ABI)
endif

# Select the Android platform to compile for
#
ifeq ($(APP_PLATFORM),)
# use a level that supports all specified ABIs if none was specified
APP_PLATFORM = android-21
endif

NDK_MAKE_TOOLCHAIN := $(NDK_ROOT)/build/tools/make_standalone_toolchain.py

#
# The source packages we want/need
# Zlib and openssl should be defined in Application.mk, libwebsockets is
# cloned from github
#

ifeq ($(ZLIB_VERSION),)
ZLIB_VERSION := 1.2.8
endif
ifeq ($(OPENSSL_VERSION),)
OPENSSL_VERSION := 1.1.1l
endif

ifeq ($(ZLIB_TGZ_SOURCE),)
ZLIB_TGZ_SOURCE := zlib-$(ZLIB_VERSION).tar.gz
endif
ifeq ($(OPENSSL_TGZ_SOURCE),)
OPENSSL_TGZ_SOURCE := openssl-$(OPENSSL_VERSION).tar.gz
endif
ifeq ($(PJSIP_SOURCE),)
PJSIP_SOURCE := ../../../../../pjproject-2.10
endif
LIBWEBSOCKETS_TGZ_SOURCE := libwebsockets.tar.gz

# The names of the directories in the source tgz files
ZLIB_DIR := $(basename $(basename $(ZLIB_TGZ_SOURCE)))
OPENSSL_DIR := $(basename $(basename $(OPENSSL_TGZ_SOURCE)))
PJSIP_DIR := pjproject-2.10
LIBWEBSOCKETS_DIR := $(basename $(basename $(LIBWEBSOCKETS_TGZ_SOURCE)))

# The URLs used to fetch the source tgz files
ZLIB_TGZ_URL := http://prdownloads.sourceforge.net/libpng/$(ZLIB_TGZ_SOURCE)
OPENSSL_TGZ_URL := https://openssl.org/source/$(OPENSSL_TGZ_SOURCE)
PJSIP_TGZ_URL := ../../../../../$(PJSIP_VERSION).tar.gz

ifeq ($(LIBWEBSOCKETS_GIT_URL),)
LIBWEBSOCKETS_GIT_URL := https://github.com/warmcat/libwebsockets.git
endif

# These values are the same as the values for $TARGET_ARCH_ABI in Android.mk
# This way 'make $TARGET_ARCH_ABI' builds libraries for that ABI.
# This is also the name for the directory where the libraries are installed to.
#
TARGET_ARM := armeabi
TARGET_ARM_V7A := armeabi-v7a
TARGET_ARM64_V8A := arm64-v8a

# The Android NDK API version to build the libraries with.
#
#  android-9 ... android-19 support arm mips and x86
#  android-21 and higher also support arm64 mips64 and x86_64
#
# These should be set to the same value as APP_PLATFORM (Application.mk)
#
# http://developer.android.com/ndk/guides/stable_apis.html
#
# If you change these or APP_PLATFORM you must do a 'make clean'
#
# Note:
# libraries compiled for android-21 and upwards are incompatible with devices below that version!
# http://stackoverflow.com/questions/28740315/android-ndk-getting-java-lang-unsatisfiedlinkerror-dlopen-failed-cannot-loca
#
TARGET_ARM_NDK_API := $(subst android-,,$(APP_PLATFORM))
TARGET_ARM_V7A_NDK_API := $(subst android-,,$(APP_PLATFORM))
TARGET_ARM64_V8A_NDK_API := $(subst android-,,$(APP_PLATFORM))

# The configure arguments to pass to the OpenSSL Configure script
# (--prefix and --openssldir are added automaticaly).
# (note: use no-asm on x86 and x86_64 to generate fully position independent code)
#
# armeabi
TARGET_ARM_OPENSSL_CONFIG_TARGET := android-arm
TARGET_ARM_OPENSSL_CONFIG := no-shared no-idea no-mdc2 no-rc5 no-zlib no-zlib-dynamic no-ssl2 no-ssl3 enable-ec enable-ecdh
# armeabi-v7a
TARGET_ARM_V7A_OPENSSL_CONFIG_TARGET := android-arm
TARGET_ARM_V7A_OPENSSL_CONFIG := no-shared no-idea no-mdc2 no-rc5 no-zlib no-zlib-dynamic no-ssl2 no-ssl3 enable-ec enable-ecdh 
# arm64-v8a
TARGET_ARM64_V8A_OPENSSL_CONFIG_TARGET := android-arm64
TARGET_ARM64_V8A_OPENSSL_CONFIG := no-shared no-idea no-mdc2 no-rc5 no-zlib no-zlib-dynamic no-ssl2 no-ssl3 enable-ec enable-ecdh

# The cmake configuration options for libwebsockets per target ABI,
# --prefix and openssl library/header paths are set automaticaly and
# the location of zlib should be picked up by CMake
# armeabi
TARGET_ARM_LWS_OPTIONS = \
 -DCMAKE_C_COMPILER=$(shell pwd)/$(TOOLCHAIN_ARM)/bin/$(TOOLCHAIN_ARM_PREFIX)-gcc \
 -DCMAKE_AR=$(shell pwd)/$(TOOLCHAIN_ARM)/bin/$(TOOLCHAIN_ARM_PREFIX)-ar \
 -DCMAKE_RANLIB=$(shell pwd)/$(TOOLCHAIN_ARM)/bin/$(TOOLCHAIN_ARM_PREFIX)-ranlib \
 -DCMAKE_C_FLAGS="$$CFLAGS" \
 -DLWS_WITH_SHARED=OFF \
 -DLWS_WITH_STATIC=ON \
 -DLWS_WITHOUT_DAEMONIZE=ON \
 -DLWS_WITHOUT_TESTAPPS=ON \
 -DLWS_IPV6=OFF \
 -DLWS_WITH_BUNDLED_ZLIB=OFF \
 -DLWS_WITH_SSL=ON  \
 -DLWS_WITH_HTTP2=ON \
 -DCMAKE_BUILD_TYPE=Release
# armeabi-v7a
TARGET_ARM_V7A_LWS_OPTIONS = \
 -DCMAKE_C_COMPILER=$(shell pwd)/$(TOOLCHAIN_ARM_V7A)/bin/$(TOOLCHAIN_ARM_V7A_PREFIX)-gcc \
 -DCMAKE_AR=$(shell pwd)/$(TOOLCHAIN_ARM_V7A)/bin/$(TOOLCHAIN_ARM_V7A_PREFIX)-ar \
 -DCMAKE_RANLIB=$(shell pwd)/$(TOOLCHAIN_ARM_V7A)/bin/$(TOOLCHAIN_ARM_V7A_PREFIX)-ranlib \
 -DCMAKE_C_FLAGS="$$CFLAGS" \
 -DLWS_WITH_SHARED=OFF \
 -DLWS_WITH_STATIC=ON \
 -DLWS_WITHOUT_DAEMONIZE=ON \
 -DLWS_WITHOUT_TESTAPPS=ON \
 -DLWS_IPV6=OFF \
 -DLWS_WITH_BUNDLED_ZLIB=OFF \
 -DLWS_WITH_SSL=ON  \
 -DLWS_WITH_HTTP2=ON \
 -DCMAKE_BUILD_TYPE=Release
# arm64-v8a
TARGET_ARM64_V8A_LWS_OPTIONS = \
 -DCMAKE_C_COMPILER=$(shell pwd)/$(TOOLCHAIN_ARM64_V8A)/bin/$(TOOLCHAIN_ARM64_V8A_PREFIX)-gcc \
 -DCMAKE_AR=$(shell pwd)/$(TOOLCHAIN_ARM64_V8A)/bin/$(TOOLCHAIN_ARM64_V8A_PREFIX)-ar \
 -DCMAKE_RANLIB=$(shell pwd)/$(TOOLCHAIN_ARM64_V8A)/bin/$(TOOLCHAIN_ARM64_V8A_PREFIX)-ranlib \
 -DCMAKE_C_FLAGS="$$CFLAGS" \
 -DLWS_WITH_SHARED=OFF \
 -DLWS_WITH_STATIC=ON \
 -DLWS_WITHOUT_DAEMONIZE=ON \
 -DLWS_WITHOUT_TESTAPPS=ON \
 -DLWS_IPV6=OFF \
 -DLWS_WITH_BUNDLED_ZLIB=OFF \
 -DLWS_WITH_SSL=ON  \
 -DLWS_WITH_HTTP2=ON \
 -DCMAKE_BUILD_TYPE=Release

#
# Toolchain configuration
#

# The directory names for the different toolchains
TOOLCHAIN_ARM := toolchains/arm
TOOLCHAIN_ARM_V7A := toolchains/arm-v7a
TOOLCHAIN_ARM64_V8A := toolchains/arm64-v8a

# Use APP_STL to determine what STL to use.
#
ifeq ($(APP_STL),stlport_static)
TOOLCHAIN_STL := stlport
else ifeq ($(APP_STL),stlport_shared)
TOOLCHAIN_STL := stlport
else ifeq ($(APP_STL),gnustl_static)
TOOLCHAIN_STL := gnustl
else ifeq ($(APP_STL),gnustl_shared)
TOOLCHAIN_STL := gnustl
else ifeq ($(APP_STL),c++_static)
TOOLCHAIN_STL := libc++
else ifeq ($(APP_STL),c++_shared)
TOOLCHAIN_STL := libc++
endif

# The settings to use for the individual toolchains:
# arm
TOOLCHAIN_ARM_API := $(TARGET_ARM_NDK_API)
TOOLCHAIN_ARM_PREFIX := arm-linux-androideabi
TOOLCHAIN_ARM_FLAGS := -mthumb
TOOLCHAIN_ARM_LINK :=
TOOLCHAIN_ARM_PLATFORM_HEADERS := $(shell pwd)/$(TOOLCHAIN_ARM)/sysroot/usr/include
TOOLCHAIN_ARM_PLATFORM_LIBS := $(shell pwd)/$(TOOLCHAIN_ARM)/sysroot/usr/lib
# arm-v7a
TOOLCHAIN_ARM_V7A_API := $(TARGET_ARM_V7A_NDK_API)
TOOLCHAIN_ARM_V7A_PREFIX := arm-linux-androideabi
TOOLCHAIN_ARM_V7A_FLAGS := -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16
TOOLCHAIN_ARM_V7A_LINK := -march=armv7-a -Wl,--fix-cortex-a8
TOOLCHAIN_ARM_V7A_PLATFORM_HEADERS :=  $(shell pwd)/$(TOOLCHAIN_ARM_V7A)/sysroot/usr/include
TOOLCHAIN_ARM_V7A_PLATFORM_LIBS := $(shell pwd)/$(TOOLCHAIN_ARM_V7A)/sysroot/usr/lib
# arm64-v8a
TOOLCHAIN_ARM64_V8A_API := $(TARGET_ARM64_V8A_NDK_API)
TOOLCHAIN_ARM64_V8A_PREFIX := aarch64-linux-android
TOOLCHAIN_ARM64_V8A_FLAGS :=
TOOLCHAIN_ARM64_V8A_LINK := -Wl
TOOLCHAIN_ARM64_V8A_PLATFORM_HEADERS := $(shell pwd)/$(TOOLCHAIN_ARM64_V8A)/sysroot/usr/include
TOOLCHAIN_ARM64_V8A_PLATFORM_LIBS := $(shell pwd)/$(TOOLCHAIN_ARM64_V8A)/sysroot/usr/lib


# Environment variables to set while compiling for each ABI
# arm
TOOLCHAIN_ARM_ENV = \
 ANDROID_DEV="$(shell pwd)/$(TOOLCHAIN_ARM)/bin" \
 CC=$(TOOLCHAIN_ARM_PREFIX)-gcc \
 CXX=$(TOOLCHAIN_ARM_PREFIX)-g++ \
 LINK=$(TOOLCHAIN_ARM_PREFIX)-g++ \
 LD=$(TOOLCHAIN_ARM_PREFIX)-ld \
 AR=$(TOOLCHAIN_ARM_PREFIX)-ar \
 RANLIB=$(TOOLCHAIN_ARM_PREFIX)-ranlib \
 STRIP=$(TOOLCHAIN_ARM_PREFIX)-strip \
 ARCH_FLAGS="$(TOOLCHAIN_ARM_FLAGS)" \
 ARCH_LINK="$(TOOLCHAIN_ARM_LINK)" \
 CPPFLAGS="-I. $(TOOLCHAIN_ARM_FLAGS) -I$(TOOLCHAIN_ARM_PLATFORM_HEADERS) -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64" \
 CXXFLAGS="-I. $(TOOLCHAIN_ARM_FLAGS) -I$(TOOLCHAIN_ARM_PLATFORM_HEADERS) -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 -frtti -fexceptions" \
 CFLAGS="-I. $(TOOLCHAIN_ARM_FLAGS) -I$(TOOLCHAIN_ARM_PLATFORM_HEADERS) -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64" \
 LDFLAGS="$(TOOLCHAIN_ARM_LINK)" \
 PATH="$(shell pwd)/$(TOOLCHAIN_ARM)/bin:$$PATH"

ANDROID_NDK_ARM="$(shell pwd)/$(TOOLCHAIN_ARM)"

# arm-v7a
TOOLCHAIN_ARM_V7A_ENV = \
 ANDROID_DEV="$(shell pwd)/$(TOOLCHAIN_ARM_V7A)/bin" \
 CC=$(TOOLCHAIN_ARM_V7A_PREFIX)-gcc \
 CXX=$(TOOLCHAIN_ARM_V7A_PREFIX)-g++ \
 LINK=$(TOOLCHAIN_ARM_V7A_PREFIX)-g++ \
 LD=$(TOOLCHAIN_ARM_V7A_PREFIX)-ld \
 AR=$(TOOLCHAIN_ARM_V7A_PREFIX)-ar \
 RANLIB=$(TOOLCHAIN_ARM_V7A_PREFIX)-ranlib \
 STRIP=$(TOOLCHAIN_ARM_V7A_PREFIX)-strip \
 ARCH_FLAGS="$(TOOLCHAIN_ARM_V7A_FLAGS)" \
 ARCH_LINK="$(TOOLCHAIN_ARM_V7A_LINK)" \
 CPPFLAGS="-I. $(TOOLCHAIN_ARM_V7A_FLAGS) -I$(TOOLCHAIN_ARM_V7A_PLATFORM_HEADERS) -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64" \
 CXXFLAGS="-I. $(TOOLCHAIN_ARM_V7A_FLAGS) -I$(TOOLCHAIN_ARM_V7A_PLATFORM_HEADERS) -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 -frtti -fexceptions" \
 CFLAGS="-I. $(TOOLCHAIN_ARM_V7A_FLAGS) -I$(TOOLCHAIN_ARM_V7A_PLATFORM_HEADERS) -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64" \
 LDFLAGS="$(TOOLCHAIN_ARM_V7A_LINK)" \
 PATH="$(shell pwd)/$(TOOLCHAIN_ARM_V7A)/bin:$$PATH"

ANDROID_NDK_ARM_V7A="$(shell pwd)/$(TOOLCHAIN_ARM_V7A)"

# arm64-v8a
TOOLCHAIN_ARM64_V8A_ENV = \
 ANDROID_DEV="$(shell pwd)/$(TOOLCHAIN_ARM64_V8A)/bin" \
 CC=$(TOOLCHAIN_ARM64_V8A_PREFIX)-gcc \
 CXX=$(TOOLCHAIN_ARM64_V8A_PREFIX)-g++ \
 LINK=$(TOOLCHAIN_ARM64_V8A_PREFIX)-g++ \
 LD=$(TOOLCHAIN_ARM64_V8A_PREFIX)-ld \
 AR=$(TOOLCHAIN_ARM64_V8A_PREFIX)-ar \
 RANLIB=$(TOOLCHAIN_ARM64_V8A_PREFIX)-ranlib \
 STRIP=$(TOOLCHAIN_ARM64_V8A_PREFIX)-strip \
 ARCH_FLAGS="$(TOOLCHAIN_ARM64_V8A_FLAGS)" \
 ARCH_LINK="$(TOOLCHAIN_ARM64_V8A_LINK)" \
 CPPFLAGS="-I. $(TOOLCHAIN_ARM64_V8A_FLAGS) -I$(TOOLCHAIN_ARM64_V8A_PLATFORM_HEADERS) -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 -Wno-error" \
 CXXFLAGS="-I. $(TOOLCHAIN_ARM64_V8A_FLAGS) -I$(TOOLCHAIN_ARM64_V8A_PLATFORM_HEADERS) -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 -frtti -fexceptions" \
 CFLAGS="-I. $(TOOLCHAIN_ARM64_V8A_FLAGS) -I$(TOOLCHAIN_ARM64_V8A_PLATFORM_HEADERS) -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64 -Wno-error" \
 LDFLAGS="$(TOOLCHAIN_ARM64_V8A_LINK)" \
 PATH="$(shell pwd)/$(TOOLCHAIN_ARM64_V8A)/bin:$$PATH"

ANDROID_NDK_ARM64="$(shell pwd)/$(TOOLCHAIN_ARM64_V8A)"

# OpenSSL environment variables without arch prefix
TOOLCHAIN_ARM_ENV_OPENSSL = \
 CC=gcc \
 CXX=g++ \
 LINK=g++ \
 LD=ld \
 AR=ar \
 RANLIB=ranlib \
 STRIP=strip

#
# All the external tools we use in this Makefile
#

AWK := awk
CD := cd
CMAKE := cmake
ECHO := echo
EGREP := egrep
GIT := git
LN := ln
MKDIR := mkdir
RM := rm
SORT := sort
TAR := tar
WGET := wget
MV := mv

#
# End of user configurable options.
#

.PHONY: \
 all \
 all-armeabi \
 all-armeabi-v7a \
 all-arm64-v8a \
 common \
 sources \
 toolchains \
 toolchain-armeabi \
 toolchain-armeabi-v7a \
 toolchain-arm64-v8a \
 zlib \
 zlib-armeabi \
 zlib-armeabi-v7a \
 zlib-arm64-v8a \
 openssl \
 openssl-armeabi \
 openssl-armeabi-v7a \
 openssl-arm64-v8a \
 pjsip \
 pjsip-armeabi \
 pjsip-armeabi-v7a \
 pjsip-arm64-v8a \
 libwebsockets \
 libwebsockets-armeabi \
 libwebsockets-armeabi-v7a \
 libwebsockets-arm64-v8a \
 clean-ndk \
 clean \
 dist-clean \
 clean-targets \
 clean-target-armeabi \
 clean-target-armeabi-v7a \
 clean-target-arm64-v8a \
 clean-sources \
 clean-source-zlib \
 clean-source-openssl \
 clean-source-pjsip \
 clean-source-libwebsockets \
 clean-toolchains \
 clean-toolchain-armeabi \
 clean-toolchain-armeabi-v7a \
 clean-toolchain-arm64-v8a \
 list-targets

# Default rule: build the libraries for all ABIs defined in NDK_APP_ABI then run ndk-build
all: $(NDK_APP_ABI)
	$(NDK_ROOT)/ndk-build clean
	$(NDK_ROOT)/ndk-build

# Libraries may also be build per ABI
all-armeabi: $(TARGET_ARM)
all-armeabi-v7a: $(TARGET_ARM_V7A)
all-arm64-v8a: $(TARGET_ARM64_V8A)

# Common rule all targets depend on
common: ../jniLibs

# These rules are called from Android.mk when executing ndk-build
$(TARGET_ARM): common zlib-armeabi openssl-armeabi pjsip-armeabi libwebsockets-armeabi
$(TARGET_ARM_V7A): common zlib-armeabi-v7a openssl-armeabi-v7a pjsip-armeabi-v7a libwebsockets-armeabi-v7a
$(TARGET_ARM64_V8A): common zlib-arm64-v8a openssl-arm64-v8a pjsip-arm64-v8a libwebsockets-arm64-v8a

#
# A rule to ensure ../jniLibs points to ../libs
# (ndk-build creates ../libs but Gradle looks for ../jniLibs)
#

../libs:
	$(MKDIR) ../libs

../jniLibs: ../libs
	$(CD) .. && $(LN) -s libs jniLibs

#
# Some rules to download the sources
#

sources: $(ZLIB_TGZ_SOURCE) $(OPENSSL_TGZ_SOURCE) $(LIBWEBSOCKETS_TGZ_SOURCE)

$(ZLIB_TGZ_SOURCE):
	$(WGET) -q $(ZLIB_TGZ_URL)

$(OPENSSL_TGZ_SOURCE):
	$(WGET) -q $(OPENSSL_TGZ_URL)

$(LIBWEBSOCKETS_TGZ_SOURCE):
	if [ -d $(LIBWEBSOCKETS_DIR) ]; then $(RM) -fr $(LIBWEBSOCKETS_DIR); fi
	$(GIT) clone $(LIBWEBSOCKETS_GIT_URL)
	$(TAR) caf $(LIBWEBSOCKETS_TGZ_SOURCE) $(LIBWEBSOCKETS_DIR)
	$(RM) -fR $(LIBWEBSOCKETS_DIR)

#
# Some rules to install the required toolchains
#

toolchains: \
 toolchain-armeabi \
 toolchain-armeabi-v7a \
 toolchain-arm64-v8a \

toolchain-armeabi: $(TOOLCHAIN_ARM)
toolchain-armeabi-v7a: $(TOOLCHAIN_ARM_V7A)
toolchain-arm64-v8a: $(TOOLCHAIN_ARM64_V8A)

$(TOOLCHAIN_ARM):
ifneq ($(TOOLCHAIN_STL),)
	$(NDK_MAKE_TOOLCHAIN) \
	  --stl $(TOOLCHAIN_STL) \
	  --api $(TOOLCHAIN_ARM_API) \
	  --arch arm \
	  --install-dir $(shell pwd)/$(TOOLCHAIN_ARM)
else
	$(NDK_MAKE_TOOLCHAIN) \
	  --api $(TOOLCHAIN_ARM_API) \
	  --arch arm \
	  --install-dir $(shell pwd)/$(TOOLCHAIN_ARM)
endif

$(TOOLCHAIN_ARM_V7A):
ifneq ($(TOOLCHAIN_STL),)
	$(NDK_MAKE_TOOLCHAIN) \
	  --stl $(TOOLCHAIN_STL) \
	  --api $(TOOLCHAIN_ARM_V7A_API) \
	  --arch arm \
	  --install-dir $(shell pwd)/$(TOOLCHAIN_ARM_V7A)
else
	$(NDK_MAKE_TOOLCHAIN) \
	  --api $(TOOLCHAIN_ARM_V7A_API) \
	  --arch arm \
	  --install-dir $(shell pwd)/$(TOOLCHAIN_ARM_V7A)
endif

$(TOOLCHAIN_ARM64_V8A):
ifneq ($(TOOLCHAIN_STL),)
	$(NDK_MAKE_TOOLCHAIN) \
	  --stl $(TOOLCHAIN_STL) \
	  --api $(TOOLCHAIN_ARM64_V8A_API) \
	  --arch arm64 \
	  --install-dir $(shell pwd)/$(TOOLCHAIN_ARM64_V8A)
else
	$(NDK_MAKE_TOOLCHAIN) \
	  --api $(TOOLCHAIN_ARM64_V8A_API) \
	  --arch arm64 \
	  --install-dir $(shell pwd)/$(TOOLCHAIN_ARM64_V8A)
endif

#
# Rules to build zlib
#

zlib: \
 zlib-armeabi \
 zlib-armeabi-v7a \
 zlib-arm64-v8a \

zlib-armeabi: $(TARGET_ARM)/lib/libz.a
zlib-armeabi-v7a: $(TARGET_ARM_V7A)/lib/libz.a
zlib-arm64-v8a: $(TARGET_ARM64_V8A)/lib/libz.a

# Extracting/configuring sources

$(TARGET_ARM)/src/$(ZLIB_DIR): $(ZLIB_TGZ_SOURCE) $(TOOLCHAIN_ARM)
	-$(MKDIR) -p $(TARGET_ARM)/src
	$(TAR) xf $(ZLIB_TGZ_SOURCE) -C $(TARGET_ARM)/src
	$(CD) $(TARGET_ARM)/src/$(ZLIB_DIR) && $(TOOLCHAIN_ARM_ENV) \
	  ./configure --static --prefix=$(shell pwd)/$(TARGET_ARM)

$(TARGET_ARM_V7A)/src/$(ZLIB_DIR): $(ZLIB_TGZ_SOURCE) $(TOOLCHAIN_ARM_V7A)
	-$(MKDIR) -p $(TARGET_ARM_V7A)/src
	$(TAR) xf $(ZLIB_TGZ_SOURCE) -C $(TARGET_ARM_V7A)/src
	$(CD) $(TARGET_ARM_V7A)/src/$(ZLIB_DIR) && $(TOOLCHAIN_ARM_V7A_ENV) \
	  ./configure --static --prefix=$(shell pwd)/$(TARGET_ARM_V7A)

$(TARGET_ARM64_V8A)/src/$(ZLIB_DIR): $(ZLIB_TGZ_SOURCE) $(TOOLCHAIN_ARM64_V8A)
	-$(MKDIR) -p $(TARGET_ARM64_V8A)/src
	$(TAR) xf $(ZLIB_TGZ_SOURCE) -C $(TARGET_ARM64_V8A)/src
	$(CD) $(TARGET_ARM64_V8A)/src/$(ZLIB_DIR) && $(TOOLCHAIN_ARM64_V8A_ENV) \
	  ./configure --static --prefix=$(shell pwd)/$(TARGET_ARM64_V8A)

# Build/install library

$(TARGET_ARM)/lib/libz.a: $(TARGET_ARM)/src/$(ZLIB_DIR)
	$(CD) $(TARGET_ARM)/src/$(ZLIB_DIR) && $(TOOLCHAIN_ARM_ENV) $(MAKE) libz.a
	$(CD) $(TARGET_ARM)/src/$(ZLIB_DIR) && $(TOOLCHAIN_ARM_ENV) $(MAKE) install

$(TARGET_ARM_V7A)/lib/libz.a: $(TARGET_ARM_V7A)/src/$(ZLIB_DIR)
	$(CD) $(TARGET_ARM_V7A)/src/$(ZLIB_DIR) && $(TOOLCHAIN_ARM_V7A_ENV) $(MAKE) libz.a
	$(CD) $(TARGET_ARM_V7A)/src/$(ZLIB_DIR) && $(TOOLCHAIN_ARM_V7A_ENV) $(MAKE) install

$(TARGET_ARM64_V8A)/lib/libz.a: $(TARGET_ARM64_V8A)/src/$(ZLIB_DIR)
	$(CD) $(TARGET_ARM64_V8A)/src/$(ZLIB_DIR) && $(TOOLCHAIN_ARM64_V8A_ENV) $(MAKE) libz.a
	$(CD) $(TARGET_ARM64_V8A)/src/$(ZLIB_DIR) && $(TOOLCHAIN_ARM64_V8A_ENV) $(MAKE) install

#
# Rules to build OpenSSL
#

openssl: \
 openssl-armeabi \
 openssl-armeabi-v7a \
 openssl-arm64-v8a \

openssl-armeabi: $(TARGET_ARM)/lib/libssl.a
openssl-armeabi-v7a: $(TARGET_ARM_V7A)/lib/libssl.a
openssl-arm64-v8a: $(TARGET_ARM64_V8A)/lib/libssl.a

# Extracting/configuring sources

$(TARGET_ARM)/src/$(OPENSSL_DIR): $(OPENSSL_TGZ_SOURCE) $(TOOLCHAIN_ARM)
	-$(MKDIR) -p $(TARGET_ARM)/src
	$(TAR) xf $(OPENSSL_TGZ_SOURCE) -C $(TARGET_ARM)/src
	$(CD) $(TARGET_ARM)/src/$(OPENSSL_DIR) && $(TOOLCHAIN_ARM_ENV) && export ANDROID_NDK_HOME=$(ANDROID_NDK_ARM) && $(TOOLCHAIN_ARM_ENV_OPENSSL) \
	  ./Configure $(TARGET_ARM_OPENSSL_CONFIG_TARGET) \
	    --prefix=$(shell pwd)/$(TARGET_ARM) \
	    --openssldir=$(shell pwd)/$(TARGET_ARM)/lib/ssl \
	    $(TARGET_ARM_OPENSSL_CONFIG)

$(TARGET_ARM_V7A)/src/$(OPENSSL_DIR): $(OPENSSL_TGZ_SOURCE) $(TOOLCHAIN_ARM_V7A)
	-$(MKDIR) -p $(TARGET_ARM_V7A)/src
	$(TAR) xf $(OPENSSL_TGZ_SOURCE) -C $(TARGET_ARM_V7A)/src
	$(CD) $(TARGET_ARM_V7A)/src/$(OPENSSL_DIR) && $(TOOLCHAIN_ARM_V7A_ENV) && export ANDROID_NDK_HOME=$(ANDROID_NDK_ARM_V7A) && $(TOOLCHAIN_ARM_ENV_OPENSSL) \
	   ./Configure $(TARGET_ARM_V7A_OPENSSL_CONFIG_TARGET) \
	    --prefix=$(shell pwd)/$(TARGET_ARM_V7A) \
	    --openssldir=$(shell pwd)/$(TARGET_ARM_V7A)/lib/ssl \
	    $(TARGET_ARM_V7A_OPENSSL_CONFIG)

$(TARGET_ARM64_V8A)/src/$(OPENSSL_DIR): $(OPENSSL_TGZ_SOURCE) $(TOOLCHAIN_ARM64_V8A)
	-$(MKDIR) -p $(TARGET_ARM64_V8A)/src
	$(TAR) xf $(OPENSSL_TGZ_SOURCE) -C $(TARGET_ARM64_V8A)/src
	$(CD) $(TARGET_ARM64_V8A)/src/$(OPENSSL_DIR) && $(TOOLCHAIN_ARM64_V8A_ENV) && export ANDROID_NDK_HOME=$(ANDROID_NDK_ARM64) && $(TOOLCHAIN_ARM_ENV_OPENSSL) \
	  ./Configure $(TARGET_ARM64_V8A_OPENSSL_CONFIG_TARGET) \
	    --prefix=$(shell pwd)/$(TARGET_ARM64_V8A) \
	    --openssldir=$(shell pwd)/$(TARGET_ARM64_V8A)/lib/ssl \
	    $(TARGET_ARM64_V8A_OPENSSL_CONFIG)

# Build/install library
$(TARGET_ARM)/lib/libssl.a: $(TARGET_ARM)/src/$(OPENSSL_DIR)
	$(CD) $(TARGET_ARM)/src/$(OPENSSL_DIR) && export ANDROID_NDK_HOME=$(ANDROID_NDK_ARM) && $(TOOLCHAIN_ARM_ENV) $(MAKE) depend
	$(CD) $(TARGET_ARM)/src/$(OPENSSL_DIR) && export ANDROID_NDK_HOME=$(ANDROID_NDK_ARM) && $(TOOLCHAIN_ARM_ENV) $(MAKE) build_libs
	$(CD) $(TARGET_ARM)/src/$(OPENSSL_DIR) && $(ECHO) '#!/bin/sh\n\nfalse\n' > apps/openssl
	$(CD) $(TARGET_ARM)/src/$(OPENSSL_DIR) && export ANDROID_NDK_HOME=$(ANDROID_NDK_ARM) && $(TOOLCHAIN_ARM_ENV) $(MAKE) install_sw

$(TARGET_ARM_V7A)/lib/libssl.a: $(TARGET_ARM_V7A)/src/$(OPENSSL_DIR)
	$(CD) $(TARGET_ARM_V7A)/src/$(OPENSSL_DIR) && export ANDROID_NDK_HOME=$(ANDROID_NDK_ARM_V7A) && $(TOOLCHAIN_ARM_V7A_ENV) $(MAKE) depend
	$(CD) $(TARGET_ARM_V7A)/src/$(OPENSSL_DIR) && export ANDROID_NDK_HOME=$(ANDROID_NDK_ARM_V7A) && $(TOOLCHAIN_ARM_V7A_ENV) $(MAKE) build_libs
	$(CD) $(TARGET_ARM_V7A)/src/$(OPENSSL_DIR) && $(ECHO) '#!/bin/sh\n\nfalse\n' > apps/openssl
	$(CD) $(TARGET_ARM_V7A)/src/$(OPENSSL_DIR) && export ANDROID_NDK_HOME=$(ANDROID_NDK_ARM_V7A) && $(TOOLCHAIN_ARM_V7A_ENV) $(MAKE) install_sw

$(TARGET_ARM64_V8A)/lib/libssl.a: $(TARGET_ARM64_V8A)/src/$(OPENSSL_DIR)
	$(CD) $(TARGET_ARM64_V8A)/src/$(OPENSSL_DIR) && export ANDROID_NDK_HOME=$(ANDROID_NDK_ARM64) && $(TOOLCHAIN_ARM64_V8A_ENV) $(MAKE) depend
	$(CD) $(TARGET_ARM64_V8A)/src/$(OPENSSL_DIR) && export ANDROID_NDK_HOME=$(ANDROID_NDK_ARM64) && $(TOOLCHAIN_ARM64_V8A_ENV) $(MAKE) build_libs
	$(CD) $(TARGET_ARM64_V8A)/src/$(OPENSSL_DIR) && $(ECHO) '#!/bin/sh\n\nfalse\n' > apps/openssl
	$(CD) $(TARGET_ARM64_V8A)/src/$(OPENSSL_DIR) && export ANDROID_NDK_HOME=$(ANDROID_NDK_ARM64) && $(TOOLCHAIN_ARM64_V8A_ENV) $(MAKE) install_sw



#
# Rules to build Pjsip
#

pjsip: \
 pjsip-armeabi \
 pjsip-armeabi-v7a \
 pjsip-arm64-v8a \

pjsip-armeabi: $(TARGET_ARM)/lib/pjsip.a
pjsip-armeabi-v7a: $(TARGET_ARM_V7A)/lib/pjsip.a
pjsip-arm64-v8a: $(TARGET_ARM64_V8A)/lib/pjsip.a

# Extracting/configuring sources
$(TARGET_ARM)/src/$(PJSIP_DIR): $(TOOLCHAIN_ARM)
	-$(MKDIR) -p $(TARGET_ARM)/src
	cp -rf $(PJSIP_SOURCE) $(TARGET_ARM)/src
	$(CD) $(TARGET_ARM)/src/$(PJSIP_DIR) && $(TOOLCHAIN_ARM_ENV) \
	TARGET_ABI=$(TARGET_ARM) ./configure-android --with-ssl=$(shell pwd)/$(TARGET_ARM) --prefix=$(shell pwd)/$(TARGET_ARM)

$(TARGET_ARM_V7A)/src/$(PJSIP_DIR): $(TOOLCHAIN_ARM_V7A)
	-$(MKDIR) -p $(TARGET_ARM_V7A)/src
	cp -rf $(PJSIP_SOURCE) $(TARGET_ARM_V7A)/src
	$(CD) $(TARGET_ARM_V7A)/src/$(PJSIP_DIR) && $(TOOLCHAIN_ARM_V7A_ENV) \
	TARGET_ABI=$(TARGET_ARM_V7A) ./configure-android --use-ndk-cflags --with-ssl=$(shell pwd)/$(TARGET_ARM_V7A) --prefix=$(shell pwd)/$(TARGET_ARM_V7A)

$(TARGET_ARM64_V8A)/src/$(PJSIP_DIR): $(TOOLCHAIN_ARM64_V8A)
	-$(MKDIR) -p $(TARGET_ARM64_V8A)/src
	cp -rf $(PJSIP_SOURCE) $(TARGET_ARM64_V8A)/src
	$(CD) $(TARGET_ARM64_V8A)/src/$(PJSIP_DIR) && $(TOOLCHAIN_ARM64_V8A_ENV) \
	TARGET_ABI=$(TARGET_ARM64_V8A) ./configure-android --use-ndk-cflags --with-ssl=$(shell pwd)/$(TARGET_ARM64_V8A) --prefix=$(shell pwd)/$(TARGET_ARM64_V8A)

# Build/install library
$(TARGET_ARM)/lib/pjsip.a: $(TARGET_ARM)/src/$(PJSIP_DIR)
	$(CD) $(TARGET_ARM)/src/$(PJSIP_DIR) && $(TOOLCHAIN_ARM_ENV) $(MAKE) dep
	$(CD) $(TARGET_ARM)/src/$(PJSIP_DIR) && $(TOOLCHAIN_ARM_ENV) $(MAKE)
	$(CD) $(TARGET_ARM)/src/$(PJSIP_DIR) && $(TOOLCHAIN_ARM_ENV) $(MAKE) install

$(TARGET_ARM_V7A)/lib/pjsip.a: $(TARGET_ARM_V7A)/src/$(PJSIP_DIR)
	$(CD) $(TARGET_ARM_V7A)/src/$(PJSIP_DIR) && $(TOOLCHAIN_ARM_V7A_ENV) $(MAKE) dep
	$(CD) $(TARGET_ARM_V7A)/src/$(PJSIP_DIR) && $(TOOLCHAIN_ARM_V7A_ENV) $(MAKE)
	$(CD) $(TARGET_ARM_V7A)/src/$(PJSIP_DIR) && $(TOOLCHAIN_ARM_V7A_ENV) $(MAKE) install

$(TARGET_ARM64_V8A)/lib/pjsip.a: $(TARGET_ARM64_V8A)/src/$(PJSIP_DIR)
	$(CD) $(TARGET_ARM64_V8A)/src/$(PJSIP_DIR) && $(TOOLCHAIN_ARM64_V8A_ENV) $(MAKE) dep
	$(CD) $(TARGET_ARM64_V8A)/src/$(PJSIP_DIR) && $(TOOLCHAIN_ARM64_V8A_ENV) $(MAKE)
	$(CD) $(TARGET_ARM64_V8A)/src/$(PJSIP_DIR) && $(TOOLCHAIN_ARM64_V8A_ENV) $(MAKE) install

#
# Rules to build libwebsockets
#

libwebsockets: \
 libwebsockets-armeabi \
 libwebsockets-armeabi-v7a \
 libwebsockets-arm64-v8a \

libwebsockets-armeabi: $(TARGET_ARM)/lib/libwebsockets.a
libwebsockets-armeabi-v7a: $(TARGET_ARM_V7A)/lib/libwebsockets.a
libwebsockets-arm64-v8a: $(TARGET_ARM64_V8A)/lib/libwebsockets.a

# Extracting/configuring sources

$(TARGET_ARM)/src/$(LIBWEBSOCKETS_DIR): $(LIBWEBSOCKETS_TGZ_SOURCE) $(TOOLCHAIN_ARM) $(TARGET_ARM)/lib/libssl.a $(TARGET_ARM)/lib/libz.a
	-$(MKDIR) -p $(TARGET_ARM)/src
	$(TAR) xf $(LIBWEBSOCKETS_TGZ_SOURCE) -C $(TARGET_ARM)/src
	-$(MKDIR) -p $(TARGET_ARM)/src/$(LIBWEBSOCKETS_DIR)/build
	$(CD) $(TARGET_ARM)/src/$(LIBWEBSOCKETS_DIR)/build && $(TOOLCHAIN_ARM_ENV) \
	  $(CMAKE) $(TARGET_ARM_LWS_OPTIONS) \
	    -DCMAKE_INSTALL_PREFIX=$(shell pwd)/$(TARGET_ARM) \
	    -DLWS_OPENSSL_LIBRARIES="$(shell pwd)/$(TARGET_ARM)/lib/libssl.a;$(shell pwd)/$(TARGET_ARM)/lib/libcrypto.a" \
	    -DLWS_OPENSSL_INCLUDE_DIRS="$(shell pwd)/$(TARGET_ARM)/include" \
        -DLWS_HAVE_HMAC_CTX_new=1 \
        -DLWS_HAVE_EVP_MD_CTX_free=1 \
	    ..

$(TARGET_ARM_V7A)/src/$(LIBWEBSOCKETS_DIR): $(LIBWEBSOCKETS_TGZ_SOURCE) $(TOOLCHAIN_ARM_V7A) $(TARGET_ARM_V7A)/lib/libssl.a $(TARGET_ARM_V7A)/lib/libz.a
	-$(MKDIR) -p $(TARGET_ARM_V7A)/src
	$(TAR) xf $(LIBWEBSOCKETS_TGZ_SOURCE) -C $(TARGET_ARM_V7A)/src
	-$(MKDIR) -p $(TARGET_ARM_V7A)/src/$(LIBWEBSOCKETS_DIR)/build
	$(CD) $(TARGET_ARM_V7A)/src/$(LIBWEBSOCKETS_DIR)/build && $(TOOLCHAIN_ARM_V7A_ENV) \
	  $(CMAKE) $(TARGET_ARM_V7A_LWS_OPTIONS) \
	    -DCMAKE_INSTALL_PREFIX=$(shell pwd)/$(TARGET_ARM_V7A) \
	    -DLWS_OPENSSL_LIBRARIES="$(shell pwd)/$(TARGET_ARM_V7A)/lib/libssl.a;$(shell pwd)/$(TARGET_ARM_V7A)/lib/libcrypto.a" \
	    -DLWS_OPENSSL_INCLUDE_DIRS="$(shell pwd)/$(TARGET_ARM_V7A)/include" \
	    -DLWS_HAVE_HMAC_CTX_new=1 \
	    -DLWS_HAVE_EVP_MD_CTX_free=1 \
	    ..

$(TARGET_ARM64_V8A)/src/$(LIBWEBSOCKETS_DIR): $(LIBWEBSOCKETS_TGZ_SOURCE) $(TOOLCHAIN_ARM64_V8A) $(TARGET_ARM64_V8A)/lib/libssl.a $(TARGET_ARM64_V8A)/lib/libz.a
	-$(MKDIR) -p $(TARGET_ARM64_V8A)/src
	$(TAR) xf $(LIBWEBSOCKETS_TGZ_SOURCE) -C $(TARGET_ARM64_V8A)/src
	-$(MKDIR) -p $(TARGET_ARM64_V8A)/src/$(LIBWEBSOCKETS_DIR)/build
	$(CD) $(TARGET_ARM64_V8A)/src/$(LIBWEBSOCKETS_DIR)/build && $(TOOLCHAIN_ARM64_V8A_ENV) \
	  $(CMAKE) $(TARGET_ARM64_V8A_LWS_OPTIONS) \
	    -DCMAKE_INSTALL_PREFIX=$(shell pwd)/$(TARGET_ARM64_V8A) \
	    -DLWS_OPENSSL_LIBRARIES="$(shell pwd)/$(TARGET_ARM64_V8A)/lib/libssl.a;$(shell pwd)/$(TARGET_ARM64_V8A)/lib/libcrypto.a" \
	    -DLWS_OPENSSL_INCLUDE_DIRS="$(shell pwd)/$(TARGET_ARM64_V8A)/include" \
        -DLWS_HAVE_HMAC_CTX_new=1 \
        -DLWS_HAVE_EVP_MD_CTX_free=1 \
	    ..

# Build/install library

$(TARGET_ARM)/lib/libwebsockets.a: $(TARGET_ARM)/src/$(LIBWEBSOCKETS_DIR)
	$(CD) $(TARGET_ARM)/src/$(LIBWEBSOCKETS_DIR)/build && $(TOOLCHAIN_ARM_ENV) $(MAKE)
	$(CD) $(TARGET_ARM)/src/$(LIBWEBSOCKETS_DIR)/build && $(TOOLCHAIN_ARM_ENV) $(MAKE) install

$(TARGET_ARM_V7A)/lib/libwebsockets.a: $(TARGET_ARM_V7A)/src/$(LIBWEBSOCKETS_DIR)
	$(CD) $(TARGET_ARM_V7A)/src/$(LIBWEBSOCKETS_DIR)/build && $(TOOLCHAIN_ARM_V7A_ENV) $(MAKE)
	$(CD) $(TARGET_ARM_V7A)/src/$(LIBWEBSOCKETS_DIR)/build && $(TOOLCHAIN_ARM_V7A_ENV) $(MAKE) install

$(TARGET_ARM64_V8A)/lib/libwebsockets.a: $(TARGET_ARM64_V8A)/src/$(LIBWEBSOCKETS_DIR)
	$(CD) $(TARGET_ARM64_V8A)/src/$(LIBWEBSOCKETS_DIR)/build && $(TOOLCHAIN_ARM64_V8A_ENV) $(MAKE)
	$(CD) $(TARGET_ARM64_V8A)/src/$(LIBWEBSOCKETS_DIR)/build && $(TOOLCHAIN_ARM64_V8A_ENV) $(MAKE) install

#
# Some rules for housekeeping
#

clean-ndk:
	$(NDK_ROOT)/ndk-build clean

clean: clean-targets clean-toolchains

dist-clean: clean clean-sources

clean-targets: \
 clean-target-armeabi \
 clean-target-armeabi-v7a \
 clean-target-arm64-v8a \

clean-target-armeabi:
	-$(RM) -fr $(TARGET_ARM)

clean-target-armeabi-v7a:
	-$(RM) -fr $(TARGET_ARM_V7A)

clean-target-arm64-v8a:
	-$(RM) -fr $(TARGET_ARM64_V8A)


clean-sources: \
 clean-source-zlib \
 clean-source-openssl \
 clean-source-libwebsockets

clean-source-zlib:
	-$(RM) $(ZLIB_TGZ_SOURCE)

clean-source-openssl:
	-$(RM) $(OPENSSL_TGZ_SOURCE)

clean-source-libwebsockets:
	-$(RM) $(LIBWEBSOCKETS_TGZ_SOURCE)

clean-toolchains: \
 clean-toolchain-armeabi \
 clean-toolchain-armeabi-v7a \
 clean-toolchain-arm64-v8a \
	-$(RM) -fr toolchains

clean-toolchain-armeabi:
	-$(RM) -fr $(TOOLCHAIN_ARM)

clean-toolchain-armeabi-v7a:
	-$(RM) -fr $(TOOLCHAIN_ARM_V7A)

clean-toolchain-arm64-v8a:
	-$(RM) -fr $(TOOLCHAIN_ARM64_V8A)

# 'make list-targets' prints a list of all targets.
# Thanks to: http://stackoverflow.com/questions/4219255/how-do-you-get-the-list-of-targets-in-a-makefile
# Modified to allow us to include files in this Makefile.
list-targets: MAKE_NO_INCLUDES := 1
export MAKE_NO_INCLUDES
list-targets:
	@$(MAKE) -s list-targets-no-includes
list-targets-no-includes:
	@$(MAKE) -pRrq -f $(lastword $(MAKEFILE_LIST)) : 2>/dev/null | $(AWK) -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | $(SORT) | $(EGREP) -v -e '^[^[:alnum:]]' -e '^$@$$'

