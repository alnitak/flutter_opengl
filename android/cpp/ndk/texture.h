#ifndef ANDROID_TEXTURE_H
#define ANDROID_TEXTURE_H

#include <jni.h>
#include <GLES2/gl2.h>

typedef struct texture_bitmap {
    char * pixels;
    int width;
    int height;
} TextureBitmap;

void setVM(JavaVM *vm);

bool setBmpManagerEnvironment();

uint32_t* loadTexture(int* width, int* height, char const *path);


#endif //ANDROID_TEXTURE_H
