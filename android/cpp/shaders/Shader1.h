#ifndef ANDROID_SHADER1_H
#define ANDROID_SHADER1_H

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <EGL/eglplatform.h>

#include "../Shader.h"

class Shader1 : public Shader {

public:
    Shader1(int width, int height);
    virtual ~Shader1();

    bool initShader(void *args);
    void drawFrame(void *args);

private:
    EGLint width;
    EGLint height;
    GLuint programObject;

    GLint indexResolution;
    GLint indexTime;

};


#endif //ANDROID_SHADER1_H
