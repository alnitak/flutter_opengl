#ifndef ANDROID_SHADER3_H
#define ANDROID_SHADER3_H

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <EGL/eglplatform.h>

#include "../Shader.h"

class Shader3 : public Shader {

public:
    Shader3(int width, int height);
    virtual ~Shader3();

    bool initShader(void *args);
    void drawFrame(void *args);

private:
    EGLint width;
    EGLint height;
    GLuint programObject;

    GLint indexResolution;
    GLint indexTime;

};


#endif //ANDROID_SHADER3_H
