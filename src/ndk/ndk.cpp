#include <jni.h>

#include "common.h"
//#include "native.h"
#include "ffi.h"
#include "Renderer.h"

#define JAVA(X) JNIEXPORT Java_com_example_flutter_1opengl_FlutterOpenglPlugin_##X

void setVM(JavaVM *vm) {
    g_ctx_f.javaVM = vm;
}

bool setFlutterEnvironment() {
    if (g_ctx_f.javaVM->GetEnv((void **) &g_ctx_f.g_env, JNI_VERSION_1_6) != JNI_OK)
        return false;

    return true;
}

extern "C" jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    setVM(vm);
    setFlutterEnvironment();

    LOGD("ndk.cpp", "JNI INIT");

    return JNI_VERSION_1_6;
}

//extern "C" void JAVA(nativeSetSurface)(JNIEnv *jenv,
//                            jclass obj,
//                            jobject surface) {
//    if (getVisualizer() != nullptr) {
//        stopThread();
//    } else {
//        createVisualizer();
//    }
//
//    getVisualizer()->setNativeWindow(g_ctx_f.g_env, static_cast<jobject>(surface));
//}
extern "C" void JAVA(nativeSetSurface)(JNIEnv *jenv,
                                       jclass obj,
                                       jobject surface,
                                       jint width, jint height) {
    if (getRenderer() != nullptr) {
        LOGD("ndk.cpp", "Renderer already created! Stopping.");
        stopThread();
    } else {
        g_ctx_f.width = width;
        g_ctx_f.height = height;
        createRenderer(&g_ctx_f);
        getRenderer()->setNativeAndroidWindow(static_cast<jobject>(surface));
    }

}