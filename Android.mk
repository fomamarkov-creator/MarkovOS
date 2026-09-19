# ============================================
# BANACHOS v8.3 — ANDROID NDK COMPILER CONFIG
# ============================================
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := banachos_core
LOCAL_SRC_FILES := main.cpp \
                   banachos_mem.cpp \
                   banachos_hal.cpp \
                   banachos_syscalls.cpp \
                   banachos_net.cpp \
                   banachos_crypto.cpp \
                   banachos_translator.cpp \
                   banachos_injector.cpp \
                   banachos_bgui.cpp \
                   banachos_bproc.cpp \
                   banachos_bcontainer.cpp \
                   banachos_baudio.cpp

LOCAL_LDLIBS    := -llog -landroid
LOCAL_CXXFLAGS  := -std=c++20 -O3 -Wall -Wextra

include $(BUILD_SHARED_LIBRARY)
