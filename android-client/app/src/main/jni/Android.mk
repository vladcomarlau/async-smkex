# get current directory
LOCAL_PATH := $(call my-dir)

# libz.a
#
include $(CLEAR_VARS)
# LOCAL_MODULE := libz
# LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libz.a
# include $(PREBUILT_STATIC_LIBRARY)

# libssl.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libssl
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libssl.a
include $(PREBUILT_STATIC_LIBRARY)

# libcrypto.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libcrypto
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libcrypto.a
include $(PREBUILT_STATIC_LIBRARY)

# libwebsockets.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libwebsockets
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libwebsockets.a
include $(PREBUILT_STATIC_LIBRARY)

# libgsmcodec-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libg7221codec-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libg7221codec-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libgsmcodec-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libgsmcodec-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libgsmcodec-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libilbccodec-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libilbccodec-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libilbccodec-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libpj-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libpj-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libpj-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libpjlib-util-arm-unknown-linux-androideabi.a 
#
include $(CLEAR_VARS)
LOCAL_MODULE := libpjlib-util-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libpjlib-util-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libpjmedia-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libpjmedia-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libpjmedia-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libpjmedia-audiodev-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libpjmedia-audiodev-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libpjmedia-audiodev-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libpjmedia-codec-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libpjmedia-codec-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libpjmedia-codec-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libpjmedia-videodev-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libpjmedia-videodev-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libpjmedia-videodev-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libpjnath-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libpjnath-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libpjnath-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libpjsip-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libpjsip-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libpjsip-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libpjsip-simple-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libpjsip-simple-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libpjsip-simple-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libpjsip-ua-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libpjsip-ua-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libpjsip-ua-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libpjsua2-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libpjsua2-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libpjsua2-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libpjsua-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libpjsua-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libpjsua-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libresample-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libresample-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libresample-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libspeex-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libspeex-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libspeex-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libsrtp-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libsrtp-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libsrtp-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libwebrtc-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libwebrtc-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libwebrtc-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)

# libyuv-arm-unknown-linux-androideabi.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libyuv-arm-unknown-linux-androideabi
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/lib/libyuv-arm-unknown-linux-androideabi.a
include $(PREBUILT_STATIC_LIBRARY)


# libsmkex.so
#
include $(CLEAR_VARS)
LOCAL_DISABLE_FATAL_LINKER_WARNINGS := true
LOCAL_MODULE := smkex

SMKEX_FILE_LIST := $(wildcard $(LOCAL_PATH)/../../../../../MultiPathVoIPLite/smkex/src/*.cpp)
AUTO_RESEND_FILE_LIST := $(wildcard $(LOCAL_PATH)/../../../../../MultiPathVoIPLite/auto_resend/src/*.cpp)
BASE_SERVICE_FILE_LIST := $(wildcard $(LOCAL_PATH)/../../../../../MultiPathVoIPLite/base_service/src/*.cpp)
BUDDY_FILE_LIST := $(wildcard $(LOCAL_PATH)/../../../../../MultiPathVoIPLite/buddy/src/*.cpp)
CALL_MANAGER_FILE_LIST := $(wildcard $(LOCAL_PATH)/../../../../../MultiPathVoIPLite/call_manager/src/*.cpp)
CONFIG_LIST := $(wildcard $(LOCAL_PATH)/../../../../../MultiPathVoIPLite/config/src/*.cpp)
DATA_MSG_FILE_LIST := $(wildcard $(LOCAL_PATH)/../../../../../MultiPathVoIPLite/data_msg/src/*.cpp)
UTILS_FILE_LIST := $(wildcard $(LOCAL_PATH)/../../../../../MultiPathVoIPLite/utils/src/*.cpp)
SIP_STACK_FILE_LIST := $(wildcard $(LOCAL_PATH)/../../../../../MultiPathVoIPLite/sip_stack/src/*.cpp)
USER_ACCOUNT_FILE_LIST := $(wildcard $(LOCAL_PATH)/../../../../../MultiPathVoIPLite/user_account/src/*.cpp)
WEB_SOCKETS_FILE_LIST := $(wildcard $(LOCAL_PATH)/../../../../../MultiPathVoIPLite/web_sockets/src/*.cpp)
TESTS_FILE_LIST := $(wildcard $(LOCAL_PATH)/../../../../../MultiPathVoIPLite/tests/src/*.cpp) #### TODO: COMPILE THIS ####


LOCAL_SRC_FILES := \
					smkex/account/src/AccountServiceJNI.cpp \
					smkex/account/src/AccountRegCallback.cpp \
					smkex/calls/src/CallServiceJNI.cpp \
					smkex/calls/src/CallsCallback.cpp \
					smkex/messaging/src/MessageServiceJNI.cpp \
					smkex/messaging/src/MessageCallback.cpp \
					smkex/presence/src/PresenceServiceJNI.cpp \
					smkex/ndk_callback/src/AndroidCallbackJNI.cpp \
					smkex/ndk_callback/src/ObserverChain.cpp \
					smkex/ndk_callback/src/JniCallback.cpp \
					smkex/smkex/src/SmkexJNI.cpp \
					smkex/smkex/src/SmkexKeyExchangedCallback.cpp \
					smkex/smkex/src/WebSocketsAndroid.cpp \
					$(SMKEX_FILE_LIST:$(LOCAL_PATH)/%=%) \
					$(AUTO_RESEND_FILE_LIST:$(LOCAL_PATH)/%=%) \
					$(BASE_SERVICE_FILE_LIST:$(LOCAL_PATH)/%=%) \
					$(BUDDY_FILE_LIST:$(LOCAL_PATH)/%=%) \
					$(CALL_MANAGER_FILE_LIST:$(LOCAL_PATH)/%=%) \
					$(CONFIG_LIST:$(LOCAL_PATH)/%=%) \
					$(DATA_MSG_FILE_LIST:$(LOCAL_PATH)/%=%) \
					$(UTILS_FILE_LIST:$(LOCAL_PATH)/%=%) \
					$(SIP_STACK_FILE_LIST:$(LOCAL_PATH)/%=%) \
					$(USER_ACCOUNT_FILE_LIST:$(LOCAL_PATH)/%=%) \
					$(WEB_SOCKETS_FILE_LIST:$(LOCAL_PATH)/%=%) \
					$(TESTS_FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := \
					$(LOCAL_PATH) \
					$(TARGET_ARCH_ABI)/include \
					smkex/account/include \
					smkex/calls/include \
					smkex/messaging/include \
					smkex/presence/include \
					smkex/ndk_callback/include \
					smkex/smkex/include \
					../../../../../MultiPathVoIPLite/smkex/include \
					../../../../../MultiPathVoIPLite/auto_resend/include \
					../../../../../MultiPathVoIPLite/base_service/include \
					../../../../../MultiPathVoIPLite/buddy/include \
					../../../../../MultiPathVoIPLite/call_manager/include \
					../../../../../MultiPathVoIPLite/config/include \
					../../../../../MultiPathVoIPLite/data_msg/include \
					../../../../../MultiPathVoIPLite/sip_stack/include \
					../../../../../MultiPathVoIPLite/user_account/include \
					../../../../../MultiPathVoIPLite/utils/include \
					../../../../../MultiPathVoIPLite/web_sockets/include \
					../../../../../MultiPathVoIPLite/tests/include

LOCAL_STATIC_LIBRARIES := websockets \
						z \
						pjsua2-arm-unknown-linux-androideabi \
						pjsua-arm-unknown-linux-androideabi \
						pjsip-ua-arm-unknown-linux-androideabi \
						pjsip-simple-arm-unknown-linux-androideabi \
						pjsip-arm-unknown-linux-androideabi \
						pjmedia-codec-arm-unknown-linux-androideabi \
						pjmedia-arm-unknown-linux-androideabi \
						pjmedia-videodev-arm-unknown-linux-androideabi \
						pjmedia-audiodev-arm-unknown-linux-androideabi \
						pjmedia-arm-unknown-linux-androideabi \
						pjnath-arm-unknown-linux-androideabi \
						pjlib-util-arm-unknown-linux-androideabi \
						srtp-arm-unknown-linux-androideabi \
						resample-arm-unknown-linux-androideabi \
						gsmcodec-arm-unknown-linux-androideabi \
						speex-arm-unknown-linux-androideabi \
						ilbccodec-arm-unknown-linux-androideabi \
						g7221codec-arm-unknown-linux-androideabi \
						yuv-arm-unknown-linux-androideabi \
						webrtc-arm-unknown-linux-androideabi \
						pj-arm-unknown-linux-androideabi \
						ssl \
						crypto
LOCAL_LDLIBS := -llog
LOCAL_CPP_FEATURES += exceptions
LOCAL_CPPFLAGS += -fexceptions
LOCAL_CFLAGS := -DPJ_AUTOCONF=1 -DANDROID=1 
include $(BUILD_SHARED_LIBRARY)

