#ifndef ANDROID_SHADER2_H
#define ANDROID_SHADER2_H

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <EGL/eglplatform.h>

#include "../Shader.h"

class Shader2 : public Shader {

public:
    Shader2(int width, int height);
    virtual ~Shader2();

    GLuint loadImage(const char* path);

    bool initShader(void *args);
    void drawFrame(void *args);

private:
    EGLint width;
    EGLint height;
    GLuint programObject;

    GLint indexResolution;
    GLint indexTime;
    GLuint textureId;
    GLint samplerLoc;
};


#endif //ANDROID_SHADER2_H
