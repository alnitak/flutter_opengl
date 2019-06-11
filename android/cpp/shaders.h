#ifndef ANDROID_SHADERS_H
#define ANDROID_SHADERS_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include <GLES/gl.h>


// these values are set in Renderer::initializeGL()
struct ShadersCommonParams {
    EGLint width;
    EGLint height;
    // Handle to a program object
    GLuint programObject;
};

void eglPrintError(std::string note, EGLint error);

void setShadersCommonParamsWidth(GLsizei width);
void setShadersCommonParamsHeight(GLsizei height);
void setShadersCommonParamsProgram(GLuint program);
ShadersCommonParams getShadersCommonParams();

void defaultDrawFrame(void *args);
void drawFrame1(void *args);
void drawFrame2(void *args);
void drawFrame3(void *args);

bool initShaderDefaults(void *args);
bool initShader1(void *args);
bool initShader2(void *args);
bool initShader3(void *args);


#endif //ANDROID_SHADERS_H
