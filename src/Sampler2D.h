#ifndef SAMPLER2D_H
#define SAMPLER2D_H

#include "common.h"
#include <vector>

#ifdef _IS_ANDROID_
#include <GLES3/gl3.h>
#include <GLES3/gl32.h>
#elif _IS_LINUX_
#include <GL/glew.h>
#elif _IS_WIN_
#include <GL/glew.h>
#endif

class Sampler2D {
public:
    Sampler2D();
    ~Sampler2D();

    void add_RGBA32(int w, int h, unsigned char *rawData);
    void genTexture(int n);

    std::vector<unsigned char> data;
    int width, height;
    GLuint texture_index;
    // define the Nth texture. 
    // The 1st texture defined will be used as GL_TEXTURE0
    // The 2nd GL_TEXTURE1 and so on
    int nTexture; 
};

#endif // SAMPLER2D_H