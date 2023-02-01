#ifndef SAMPLER2D_H
#define SAMPLER2D_H

#include "common.h"
#include <vector>



class Sampler2D {
public:
    Sampler2D();
    ~Sampler2D();

    void add_RGBA32(int w, int h, unsigned char *rawData);
    void genTexture(int n);

    std::vector<unsigned char> data;
    int width, height;
    unsigned int texture_index;
    // define the Nth texture. 
    // The 1st texture defined will be used as GL_TEXTURE0
    // The 2nd GL_TEXTURE1 and so on
    int nTexture; 
};

#endif // SAMPLER2D_H