#ifndef COMMON_H
#define COMMON_H

#include <android/log.h>
#include <EGL/egl.h> // requires ndk r5 or newer

#define  LOG_TAG_NATIVE    "native-lib"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG_NATIVE, __VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,  LOG_TAG_NATIVE, __VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG_NATIVE, __VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG_NATIVE, __VA_ARGS__)

#endif // COMMON_H