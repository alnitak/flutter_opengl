// Copyright 2019 Marco Bavagnoli <marco.bavagnoli@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include <cstdlib>

#include "RendererController.h"
#include "common.hpp"

#define JAVA(X) JNIEXPORT Java_com_bavagnoli_opengl_JNIUtils_##X

static JavaVM* jvm = 0;
static JNIEnv *env;

RendererController *rendererController;



jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    jvm = vm;
    if (jvm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK)
        return -1;

    LOGI("JNI INIT");

    return JNI_VERSION_1_6;
}


////////////////////////////////
/// renderer
////////////////////////////////
extern "C" {
    void JAVA(nativeSetSurface)(JNIEnv *jenv, jobject obj,
                                jobject surface,
                                jint func,
                                jstring name, // MAX 50 chars
                                jint width, jint height,
                                jfloat scaleX, jfloat scaleY,
                                jint clearR, jint clearG, jint clearB,
                                jint clearA) {
    //    LOGI("nativeSetSurface1");
        const char *jName = jenv->GetStringUTFChars(name, JNI_FALSE);
        if (rendererController == NULL) {
            rendererController = new RendererController();
        }
    //    LOGI("nativeSetSurface2");

        rendererController->nativeSetSurface(jenv,
                                             surface,
                                             func,
                                             jName,
                                             width, height,
                                             scaleX, scaleY,
                                             clearR, clearG, clearB, clearA);

        jenv->ReleaseStringUTFChars(name, jName);
    //    LOGI("nativeSetSurface3 fine");
    }

    void JAVA(nativeOnStop)(JNIEnv *jenv, jobject obj) {
        if (rendererController == NULL) return;
        LOGI("nativeOnStop1");
        rendererController->nativeOnStop();
        LOGI("nativeOnStop2");
        free(rendererController);
        rendererController = NULL;
        LOGI("nativeOnStop3 fine");
    }


    int JAVA(nativeSurfaceIsContextValid)(JNIEnv *jenv, jobject obj) {
        if (rendererController == NULL) return -1;
        if (rendererController->renderer == NULL) return -1;
        return rendererController->renderer->isContextValid();
    }

    bool JAVA(nativeSurfaceIsLoopingRunning)(JNIEnv *jenv, jobject obj) {
        if (rendererController == NULL) return false;
        if (rendererController->renderer == NULL) return false;
        return rendererController->renderer->isLoopingRunning();
    }

    bool JAVA(nativeSurfaceIsThreadRunning)(JNIEnv *jenv, jobject obj) {
        if (rendererController == NULL) return false;
        if (rendererController->renderer == NULL) return false;
        return rendererController->renderer->isThreadRunning();
    }


    void JAVA(nativeSurfaceSetClearColor)(JNIEnv *jenv, jobject obj,
                                          jint clearR, jint clearG, jint clearB,
                                          jint clearA) {
        if (rendererController == NULL) return;
        if (rendererController->renderer == NULL) return;
    //    pthread_mutex_lock(&_mutex_controller);
        rendererController->renderer->setClearColor((const unsigned char) clearR,
                                                    (const unsigned char) clearG,
                                                    (const unsigned char) clearB,
                                                    (const unsigned char) clearA);
    //    pthread_mutex_unlock(&_mutex_controller);
    }

}
