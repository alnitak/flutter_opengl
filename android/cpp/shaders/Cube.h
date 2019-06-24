#ifndef ANDROID_CUBE_H
#define ANDROID_CUBE_H

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <EGL/eglplatform.h>

#include "../Shader.h"

class Cube : public Shader {

public:
    Cube(int width, int height);
    virtual ~Cube();

    bool initShader(void *args);
    void drawFrame(void *args);

private:
    EGLint width;
    EGLint height;

    GLfloat vertices[108];
    GLfloat colors[144];
};


#endif //ANDROID_CUBE_H
