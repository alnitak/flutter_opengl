#ifndef ANDROID_SHADER_H
#define ANDROID_SHADER_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <EGL/eglplatform.h>

class Shader {

public:

    virtual bool initShader(void *args) = 0;
    virtual void drawFrame(void *args) = 0;
    static void eglPrintError(std::string note, EGLint error);

    GLuint loadShader(GLenum shaderType, const char* shaderSource);
    GLuint createProgram(const char* vertexSource, const char * fragmentSource);

    void setOrtho(GLfloat *m,
                  GLfloat left, GLfloat right,
                  GLfloat bottom, GLfloat top,
                  GLfloat near, GLfloat far);
};


#endif //ANDROID_SHADER_H
