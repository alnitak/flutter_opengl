#ifndef SHADER_H
#define SHADER_H

#include "common.h"
#include "uniformQueue.h"

#include <iostream>
#include <mutex>
#include <memory>
#include <EGL/egl.h>

#ifdef _IS_ANDROID_
#define EGL_EGLEXT_PROTOTYPES

#include "EGL/eglext.h"
#include "GLES3/gl3.h"

#define GL_GLEXT_PROTOTYPES

#include "GLES3/gl3ext.h"
#include <GLES3/gl32.h>

#include "glm.hpp"

#endif
#ifdef _IS_LINUX_
#include "../linux/include/fl_my_texture_gl.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#endif
#ifdef _IS_WIN_
#include "../windows/flutter_opengl_plugin.h"
#include <GL/glew.h>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#endif


extern "C" void eglPrintError(const std::string &note);


class Shader {

public:
    Shader(OpenglPluginContext *textureStruct);

    virtual ~Shader();

    void addShaderToyUniforms();

    void setShadersSize(int w, int h);

    void setShadersText(std::string vertexSource, std::string fragmentSource);

    void setIsContinuous(bool isContinuous);

    bool isContinuous() { return _isContinuous; };

    bool isValid() { return programObject != 0; }

    bool getProgramObject() { return programObject; }

    int getWidth() { return width; }

    int getHeight() { return height; }

    std::string initShaderToy();

    std::string initShader();

    void drawFrame();

    UniformQueue &getUniforms() { return uniformsList; };

    void use() const;

    std::string compileError;
    std::string vertexSource;
    std::string fragmentSource;
private:
// good glVertexAttribPointer explaination
// https://stackoverflow.com/questions/16380005/opengl-3-4-glvertexattribpointer-stride-and-offset-miscalculation?r=Saves_UserSavesList
    const GLfloat shaderVertices[18] = {
            -1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f
    };
    mutable std::mutex mutex_;
    OpenglPluginContext *self;
    bool _isContinuous = true;
    EGLint width;
    EGLint height;
    GLuint programObject;
    GLuint VAO, VBO;
    GLuint FBO;
    GLfloat startTime;

    UniformQueue uniformsList;

    GLuint loadShader(GLenum shaderType, const char *shaderSource);

    GLuint createProgram(const char *vertexSource, const char *fragmentSource);
};


#endif //SHADER_H
