#include <math.h>

#include "Cube.h"
#include "../ndk/texture.h"



Cube::Cube(int width, int height)
    : vertices {
        // front
        -0.5F, 0.5F, 0.5F,
        -0.5F, -0.5F, 0.5F,
        0.5F, 0.5F, 0.5F,
        0.5F, 0.5F, 0.5F,
        -0.5F, -0.5F, 0.5F,
        0.5F, -0.5F, 0.5F,

        // right
        0.5F, 0.5F, 0.5F,
        0.5F, -0.5F, 0.5F,
        0.5F, 0.5F, -0.5F,
        0.5F, 0.5F, -0.5F,
        0.5F, -0.5F, 0.5F,
        0.5F, -0.5F, -0.5F,

        // back
        0.5F, 0.5F, -0.5F,
        0.5F, -0.5F, -0.5F,
        -0.5F, 0.5F, -0.5F,
        -0.5F, 0.5F, -0.5F,
        0.5F, -0.5F, -0.5F,
        -0.5F, -0.5F, -0.5F,

        // left
        -0.5F, 0.5F, -0.5F,
        -0.5F, -0.5F, -0.5F,
        -0.5F, 0.5F, 0.5F,
        -0.5F, 0.5F, 0.5F,
        -0.5F, -0.5F, -0.5F,
        -0.5F, -0.5F, 0.5F,

        // top
        -0.5F, 0.5F, -0.5F,
        -0.5F, 0.5F, 0.5F,
        0.5F, 0.5F, -0.5F,
        0.5F, 0.5F, -0.5F,
        -0.5F, 0.5F, 0.5F,
        0.5F, 0.5F, 0.5F,

        // bottom
        -0.5F, -0.5F, 0.5F,
        -0.5F, -0.5F, -0.5F,
        0.5F, -0.5F, 0.5F,
        0.5F, -0.5F, 0.5F,
        -0.5F, -0.5F, -0.5F,
        0.5F, -0.5F, -0.5F
    },

    colors
    {
    // front
    1.0F, 0.5F, 0.0F, 1.0F,
    1.0F, 0.5F, 0.0F, 1.0F,
    1.0F, 0.5F, 0.0F, 1.0F,
    1.0F, 0.5F, 0.0F, 1.0F,
    1.0F, 0.5F, 0.0F, 1.0F,
    1.0F, 0.5F, 0.0F, 1.0F,

    // right
    0.3F, 1.0F, 0.5F ,1.0F,
    0.3F, 1.0F, 0.5F ,1.0F,
    0.3F, 1.0F, 0.5F ,1.0F,
    0.3F, 1.0F, 0.5F ,1.0F,
    0.3F, 1.0F, 0.5F ,1.0F,
    0.3F, 1.0F, 0.5F ,1.0F,

    // back
    0.5F, 0.0F, 1.0F ,1.0F,
    0.5F, 0.0F, 1.0F ,1.0F,
    0.5F, 0.0F, 1.0F ,1.0F,
    0.5F, 0.0F, 1.0F ,1.0F,
    0.5F, 0.0F, 1.0F ,1.0F,
    0.5F, 0.0F, 1.0F ,1.0F,

    // left
    0.8F, 0.5F, 0.2F,1.0F,
    0.8F, 0.5F, 0.2F,1.0F,
    0.8F, 0.5F, 0.2F,1.0F,
    0.8F, 0.5F, 0.2F,1.0F,
    0.8F, 0.5F, 0.2F,1.0F,
    0.8F, 0.5F, 0.2F,1.0F,

    // top
    0.3F, 0.8F, 0.5F ,1.0F,
    0.3F, 0.8F, 0.5F ,1.0F,
    0.3F, 0.8F, 0.5F ,1.0F,
    0.3F, 0.8F, 0.5F ,1.0F,
    0.3F, 0.8F, 0.5F ,1.0F,
    0.3F, 0.8F, 0.5F ,1.0F,

    // bottom
    0.3F, 0.2F, 0.8F ,1.0F,
    0.3F, 0.2F, 0.8F ,1.0F,
    0.3F, 0.2F, 0.8F ,1.0F,
    0.3F, 0.2F, 0.8F ,1.0F,
    0.3F, 0.2F, 0.8F ,1.0F,
    0.3F, 0.2F, 0.8F ,1.0F,
    }
{
    this->width = width;
    this->height = height;


}

Cube::~Cube()
{
}

bool Cube::initShader(void *args) {
    glClearDepthf(1.0F);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (width > height)
        glScalef((float)height/(float)width, 1.0F, 1.0F);
    else
        glScalef(1.0, (float)width/(float)height, 1.0F);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    return true;
}


void Cube::drawFrame(void *args) {
    GLfloat time = (GLfloat) clock() / (GLfloat) CLOCKS_PER_SEC;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(4, GL_FLOAT, 0, colors);

    glRotatef(.1F, sin(time), 0.2F, sin(time));

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}