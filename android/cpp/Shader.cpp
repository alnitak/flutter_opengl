#include "Shader.h"
#include "ndk/common.hpp"

#define LOG_TAG_SHADER "native shader"


void Shader::eglPrintError(std::string note, EGLint error)
{
    std::string ret;
    switch(error)
    {
        case EGL_SUCCESS:               ret = "No error"; break;
        case EGL_NOT_INITIALIZED:       ret = "EGL not initialized or failed to initialize"; break;
        case EGL_BAD_ACCESS:            ret = "Resource inaccessible"; break;
        case EGL_BAD_ALLOC:             ret = "Cannot allocate resources"; break;
        case EGL_BAD_ATTRIBUTE:         ret = "Unrecognized attribute or attribute value"; break;
        case EGL_BAD_CONTEXT:           ret = "Invalid EGL context"; break;
        case EGL_BAD_CONFIG:            ret = "Invalid EGL frame buffer configuration"; break;
        case EGL_BAD_CURRENT_SURFACE:   ret = "Current surface is no longer valid"; break;
        case EGL_BAD_DISPLAY:           ret = "Invalid EGL display"; break;
        case EGL_BAD_SURFACE:           ret = "Invalid surface"; break;
        case EGL_BAD_MATCH:             ret = "Inconsistent arguments"; break;
        case EGL_BAD_PARAMETER:         ret = "Invalid argument"; break;
        case EGL_BAD_NATIVE_PIXMAP:     ret = "Invalid native pixmap"; break;
        case EGL_BAD_NATIVE_WINDOW:     ret = "Invalid native window"; break;
        case EGL_CONTEXT_LOST:          ret = "Context lost"; break;
        default: ret = "Unknown error";
    }
    if (error == EGL_SUCCESS)
        LOGD(LOG_TAG_SHADER,"%s  %s", note.c_str(), ret.c_str());
    else
        LOGE(LOG_TAG_SHADER,"%s  error: %d  0x%X  %s", note.c_str(), error, error, ret.c_str());
}


GLuint Shader::loadShader(GLenum shaderType, const char* shaderSource)
{
    GLuint shader = glCreateShader(shaderType);
    if (shader)
    {
        glShaderSource(shader, 1, &shaderSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen)
            {
                char * buf = (char*) malloc(infoLen);
                if (buf)
                {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE(LOG_TAG_SHADER,"Could not Compile Shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint Shader::createProgram(const char* vertexSource, const char * fragmentSource)
{
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader)
        return 0;
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader)
        return 0;

    GLuint program = glCreateProgram();
    if (program)
    {
        glAttachShader(program , vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program , GL_LINK_STATUS, &linkStatus);
        if( linkStatus != GL_TRUE)
        {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength)
            {
                char* buf = (char*) malloc(bufLength);
                if (buf)
                {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE(LOG_TAG_SHADER,"Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}



void Shader::setOrtho(GLfloat *m, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near,
              GLfloat far)
{
    GLfloat tx = - (right + left)/(right - left);
    GLfloat ty = - (top + bottom)/(top - bottom);
    GLfloat tz = - (far + near)/(far - near);

    m[0] = 2.0f/(right-left);
    m[1] = 0;
    m[2] = 0;
    m[3] = tx;

    m[4] = 0;
    m[5] = 2.0f/(top-bottom);
    m[6] = 0;
    m[7] = ty;

    m[8] = 0;
    m[9] = 0;
    m[10] = -2.0f/(far-near);
    m[11] = tz;

    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 1;

//    ( 2.0/768.0, 0.0,        0.0,    -1.0,
//      0.0,       2.0/1024.0, 0.0,    -1.0,
//      0.0,       0.0,        -1.0,   0.0,
//      0.0,       0.0,        0.0,    1.0);
}