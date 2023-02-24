#ifndef COMMON_H
#define COMMON_H

// On Android, also __linux__ is defined. So make my own
#ifdef __ANDROID__
    #define _IS_ANDROID_ 1
    #define WITH_OPENCV
#elif __linux__
    #define _IS_LINUX_ 1
    #define WITH_OPENCV
#elif _WIN32 | _WIN64
    #define _IS_WIN_ 1
    #define WITH_OPENCV
#endif

#ifdef _IS_ANDROID_

#include <android/log.h>
#include <jni.h>
#include <EGL/egl.h>

#define LOGD(TAG, ...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

#define FFI_PLUGIN_EXPORT __attribute__((visibility("default"))) __attribute__((used))

typedef struct flutter_opengl_plugin_context {
    JavaVM *javaVM;
    JNIEnv *g_env;
    jclass cls;
    ANativeWindow *window;
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
    unsigned int texture_name;
    int width;
    int height;
} OpenglPluginContext;
static OpenglPluginContext g_ctx_f = {
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        0,
        0,
        0};
#elif _IS_LINUX_
#include <flutter_linux/flutter_linux.h>
#include "../linux/include/fl_my_texture_gl.h"
#include <iostream>
#include <gtk/gtk.h>
#include <glib-object.h>
#include <EGL/egl.h>
// #define  LOGD(TAG, ...) std::cout<<TAG, std::cout<<" - ", std::cout<<__VA_ARGS__, std::cout<<std::endl
#define LOGD(TAG,...) printf(TAG),printf(" "),printf(__VA_ARGS__),printf("\n");fflush(stdout);

#define FFI_PLUGIN_EXPORT __attribute__((visibility("default"))) __attribute__((used))

typedef struct flutter_opengl_plugin_context
{
    GdkGLContext *context;
    unsigned int texture_name;
    FlTextureRegistrar *texture_registrar = nullptr;
    FlMyTextureGL *myTexture = nullptr;
    g_autoptr(FlTexture) texture;
    int width;
    int height;
} OpenglPluginContext;
static OpenglPluginContext ctx_f = {
        nullptr,
        0,
        nullptr,
        nullptr,
        nullptr,
        0,
        0};

#elif _IS_WIN_
#include <flutter/plugin_registrar_windows.h>
#include "../windows/fl_my_texture_gl.h"
#include <EGL/egl.h>
#define LOGD(TAG,...) printf(TAG),printf(" "),printf(__VA_ARGS__),printf("\n");fflush(stdout);
#define FFI_PLUGIN_EXPORT __declspec(dllexport)

typedef struct flutter_opengl_plugin_context
{
    HWND m_hWnd;
    HDC hdc;
    HGLRC hrc;           // rendering context variable
    flutter::TextureRegistrar *texture_registrar;
    flutter::PluginRegistrarWindows *registrar;
    FlMyTextureGL *myTexture;
    unsigned int texture_name;
    int64_t texture_id;
    int width;
    int height;
} OpenglPluginContext;
static OpenglPluginContext ctx_f = {0, 0, 0, 0, 0, 0, 0, 0, 0};

#endif


#endif // COMMON_H