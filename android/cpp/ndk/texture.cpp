#include <jni.h>
#include <sys/types.h>
#include <malloc.h>
#include "texture.h"
#include "common.hpp"

#define LOG_TAG_TEXTURE "native texture"

typedef struct bitmap_manager_context {
    JavaVM  *javaVM;
    JNIEnv *g_env;
    jclass cls;
    jmethodID classInit;
    jmethodID mOpenId;
    jmethodID mGetWidthId;
    jmethodID mGetHeightId;
    jmethodID mGetPixelsId;
    jmethodID mCloseId;
} BitmapManagerContext;
static BitmapManagerContext g_ctx = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

void setVM(JavaVM *vm) {
    g_ctx.javaVM = vm;
}

bool setBmpManagerEnvironment() {
    if (g_ctx.javaVM->GetEnv((void**)&g_ctx.g_env, JNI_VERSION_1_6) != JNI_OK)
        return false;

    g_ctx.cls = g_ctx.g_env->FindClass("com/bavagnoli/opengl/BmpManager");
    g_ctx.cls = reinterpret_cast<jclass>(g_ctx.g_env->NewGlobalRef(g_ctx.cls));
    if (g_ctx.cls == nullptr)
        return false;

    return true;
}

uint32_t* loadTexture( int* width, int* height, char const *path) {
    uint32_t* RGBAPixels = nullptr;
    g_ctx.javaVM->AttachCurrentThread(&g_ctx.g_env, NULL);

    g_ctx.classInit = g_ctx.g_env->GetMethodID(g_ctx.cls, "<init>", "()V");  // FIND AN OBJECT CONSTRUCTOR
    if(g_ctx.classInit == nullptr)
        return nullptr;

    g_ctx.mOpenId = g_ctx.g_env->GetMethodID(g_ctx.cls, "open",
                                             "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
    if (g_ctx.mOpenId == nullptr)
        return nullptr;

    g_ctx.mGetWidthId = g_ctx.g_env->GetMethodID(g_ctx.cls, "getWidth", "(Landroid/graphics/Bitmap;)I");
    if (g_ctx.mGetWidthId == nullptr)
        return nullptr;

    g_ctx.mGetHeightId = g_ctx.g_env->GetMethodID(g_ctx.cls, "getHeight", "(Landroid/graphics/Bitmap;)I");
    if (g_ctx.mGetHeightId == nullptr)
        return nullptr;

    g_ctx.mGetPixelsId = g_ctx.g_env->GetMethodID(g_ctx.cls, "getPixels", "(Landroid/graphics/Bitmap;[I)V");
    if (g_ctx.mGetPixelsId == nullptr)
        return nullptr;

    g_ctx.mCloseId = g_ctx.g_env->GetMethodID(g_ctx.cls, "close", "(Landroid/graphics/Bitmap;)V");
    if (g_ctx.mCloseId == nullptr)
        return nullptr;

    // create BmpManager by finding and invoking a constructor
    jobject myo = g_ctx.g_env->NewObject(g_ctx.cls, g_ctx.classInit);

    // Ask the Bitmap manager for a bitmap
    jstring name = g_ctx.g_env->NewStringUTF(path);
    jobject bmp = g_ctx.g_env->CallObjectMethod(myo, g_ctx.mOpenId, name);
    g_ctx.g_env->DeleteLocalRef(name);
    g_ctx.g_env->NewGlobalRef(bmp);

    // Get image dimensions
    *width = g_ctx.g_env->CallIntMethod(myo, g_ctx.mGetWidthId, bmp);
    *height = g_ctx.g_env->CallIntMethod(myo, g_ctx.mGetHeightId, bmp);

    // Get pixels
    jintArray array = g_ctx.g_env->NewIntArray(*width * *height);
    g_ctx.g_env->NewGlobalRef(array);
    g_ctx.g_env->CallVoidMethod(myo, g_ctx.mGetPixelsId, bmp, array);

    jint *pixels = g_ctx.g_env->GetIntArrayElements(array, 0);
    if (pixels == nullptr)
        return nullptr;



    // convert ARGB_8888 to ABGR
    // OpenGL uses RGBA, but the conversion ARGB=>RGBA doesn't work!
    // TODO maybe check here if the bitmap sizes are 2^N
    RGBAPixels = (uint32_t*)malloc(sizeof(uint32_t) * *width * *height);
    for (int k=0; k<*width * *height; k++) {
        RGBAPixels[k] = ( (pixels[k] & 0xFF000000)      ) |
                        ( (pixels[k] & 0x00FF0000) >> 16 ) |
                        ( (pixels[k] & 0x0000FF00)       ) |
                        ( (pixels[k] & 0x000000FF) << 16 );
    }

    // release memory
    g_ctx.g_env->ReleaseIntArrayElements(array, pixels, 0);
    g_ctx.g_env->DeleteLocalRef(array);

    // Free image
    g_ctx.g_env->CallVoidMethod(myo, g_ctx.mCloseId, bmp);
    g_ctx.g_env->DeleteLocalRef(bmp);

    g_ctx.javaVM->DetachCurrentThread();

    LOGD(LOG_TAG_TEXTURE, "loadTexture() %dx%d ", *width, *height);
    return RGBAPixels;
}
