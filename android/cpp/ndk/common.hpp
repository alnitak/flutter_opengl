#ifndef COMMON_H
#define COMMON_H

#include <android/log.h>

#define  LOG_TAG_NATIVE    "native-lib"
#define  LOGE(TAG, ...)  __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define  LOGW(TAG, ...)  __android_log_print(ANDROID_LOG_WARN,  TAG, __VA_ARGS__)
#define  LOGD(TAG, ...)  __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define  LOGI(TAG, ...)  __android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__)

#endif // COMMON_H