#include "Shader3.h"


Shader3::Shader3(int width, int height)
{
    this->width = width;
    this->height = height;
    this->programObject = 0;
}

Shader3::~Shader3()
{
    glDeleteProgram(programObject);
}



bool Shader3::initShader(void *args) {
//    https://www.shadertoy.com/view/ttlGDf
    char vShaderStr[] =
            "precision mediump float;\n"
            "precision mediump int;\n"

            "attribute vec4 a_Position;         \n"     // Per-vertex position information we will pass in.

            "uniform vec3  u_resolution;       \n"
            "uniform float u_time;             \n"

            "varying vec3       v_resolution;  \n"     // viewport resolution (in pixels)
            "varying float      v_time;        \n"

            "void main()                       \n"     // The entry point for our vertex shader.
            "{                                 \n"
            "   gl_Position = a_Position;      \n"
            "   v_resolution = u_resolution;   \n"
            "   v_time = u_time;               \n"
            "}                                 \n";
    char fShaderStr[] =
            "precision mediump float;\n"
            "precision mediump int;\n"

            "varying vec3      v_resolution; \n"     // viewport resolution (in pixels)
            "varying float     v_time;       \n"     // shader playback time (in seconds)
            ""
            "void main()\n"
            "{\n"
            //"    //Normalized pixel coordinates (from 0 to 1)\n"
            "    vec2 uv = gl_FragCoord.xy/v_resolution.xy;\n"
            "    float t = (v_time)/3.0;\n"
            "    \n"
            "    vec2 pos = uv;\n"
            "    pos.y /= v_resolution.x/v_resolution.y;\n"
            "    pos = 4.0*(vec2(0.5,0.5) - pos);\n"
            "    \n"
            "    float strength = 0.4;\n"
            "    for(float i = 1.0; i < 7.0; i+=1.0){ \n"
            "       pos.x += strength * sin(2.0*t+i*1.5 * pos.y)+t*0.1;\n"
            "       pos.y += strength * cos(2.0*t+i*1.5 * pos.x);\n"
            "    }\n"
            "\n"
            //"    //Time varying pixel colour\n"
            "    vec3 col = 0.5 + 0.5*cos(v_time+pos.xyx+vec3(0,2,4));\n"
            "\n"
            //"    //Fragment colour\n"
            "    gl_FragColor = vec4(col,1.0);\n"
            "}";


    programObject = createProgram(vShaderStr, fShaderStr);
    // Bind to attributes
    glBindAttribLocation(programObject, 1, "u_resolution");
    glBindAttribLocation(programObject, 2, "u_time");

    // get indexes to pass to glEnableVertexAttribArray()
    indexResolution = glGetUniformLocation(programObject, "u_resolution");
    indexTime       = glGetUniformLocation(programObject, "u_time");

    // Use the program object
    glUseProgram ( programObject );

    return true;
}

void Shader3::drawFrame(void *args) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat time = (GLfloat) clock() / (GLfloat) CLOCKS_PER_SEC;

    float shaderVertices[] = {
            -1.0F,  -1.0F,  0.0F,
            -1.0F,   1.0F,  0.0F,
            1.0F,  -1.0F,  0.0F,
            -1.0F,   1.0F,  0.0F,
            1.0F,   1.0F,  0.0F,
            1.0F,  -1.0F,  0.0F
    };

    // Load the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, shaderVertices);
    glEnableVertexAttribArray(0);

    float t = time - ((int)(time / 30.0F)) * 30.0F; // pass value from 0 to 30 range
    glUniform1f(indexTime, t);
    glUniform3f(indexResolution, (GLfloat) width, (GLfloat) height, 0.0F);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

