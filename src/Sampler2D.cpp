#include "Sampler2D.h"
#include "Shader.h"

#ifdef _IS_ANDROID_
#include <GLES3/gl3.h>
#include <GLES3/gl32.h>
#elif _IS_LINUX_
#include <GL/glew.h>
#elif _IS_WIN_
#include <GL/glew.h>
#endif

#include <cstring>

Sampler2D::Sampler2D() 
    : nTexture(-1)
{
}

void Sampler2D::add_RGBA32(int w, int h, unsigned char *rawData)
{
    width = w;
    height = h;
    int size = width * height * sizeof(unsigned char) * 4;
    data.resize(size);
    memcpy(&data[0], rawData, size);
}

void Sampler2D::genTexture(int n)
{
    // if the data is empty, the texture hass already been generated
    if (data.empty()) return;

    // if nTexture != -1 the texture has already been created 
    // and we are here only to update it
    if (nTexture == -1) {
        nTexture = n;
        glGenTextures(1, &texture_index);
    }
    glActiveTexture(GL_TEXTURE0 + nTexture);
    glBindTexture(GL_TEXTURE_2D, texture_index);

    // filtering and wrap modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)data.data());
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, (void*)data.data());
    // glGenerateMipmap(GL_TEXTURE_2D);
    // glBindTexture(GL_TEXTURE_2D, texture_index);

    data.clear();
}