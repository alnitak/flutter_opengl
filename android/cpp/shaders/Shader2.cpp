#include "../ndk/common.hpp"
#include "Shader2.h"
#include "../ndk/texture.h"


Shader2::Shader2(int width, int height)
{
    this->width = width;
    this->height = height;
    this->programObject = 0;
}

Shader2::~Shader2()
{
    glDeleteProgram(programObject);
    glDeleteTextures(1, &textureId);
}

GLuint Shader2::loadImage(const char* path) {
    int w,h;
    GLuint textureId;
    uint32_t * pixels = loadTexture(&w, &h, path);
    if (pixels == nullptr)
        return false;
    // Generate a texture object
    glGenTextures(1, &textureId);
    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, textureId);
    // Load mipmap level 0
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    // free bitmap memory when not needed anymore
    free(pixels);
    // Set the filtering mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureId;
}

bool Shader2::initShader(void *args) {
//    https://www.shadertoy.com/view/llj3Dz
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
            "uniform sampler2D s_texture;    \n"
            "#define t v_time\n"
            "#define r v_resolution.xy\n"
            ""
            "void main(){\n"
            //Sawtooth function to pulse from centre.
            "   float offset = (v_time- floor(v_time))/v_time;\n"
            "   float CurrentTime = (v_time)*(offset);    \n"
            ""
            "   vec3 WaveParams = vec3(10.0, 4.8, 0.1); \n"
            ""
            "    float ratio = v_resolution.y/v_resolution.x * -1.0;\n"
            ""
            "    //Use this if you want to place the centre with the mouse instead\n"
            //"    //vec2 WaveCentre = vec2( iMouse.xy / v_resolution.xy );\n"
            ""
            "    vec2 WaveCentre = vec2(0.5, 0.5);\n"
            "    WaveCentre.y *= ratio; \n"
            ""
            "   vec2 texCoord = gl_FragCoord.xy / v_resolution.xy;      \n"
            "    texCoord.y *= ratio;    \n"
            "   float Dist = distance(texCoord, WaveCentre);\n"
            ""
            ""
            "   vec4 Color = texture2D(s_texture, texCoord);\n"
            ""
            "    //Only distort the pixels within the parameter distance from the centre\n"
            "    if ((Dist <= ((CurrentTime) + (WaveParams.z))) && \n"
            "        (Dist >= ((CurrentTime) - (WaveParams.z)))) \n"
            "   {\n"
            "        //The pixel offset distance based on the input parameters\n"
            "       float Diff = (Dist - CurrentTime); \n"
            "       float ScaleDiff = (1.0 - pow(abs(Diff * WaveParams.x), WaveParams.y)); \n"
            "       float DiffTime = (Diff  * ScaleDiff);\n"
            ""
            "        //The direction of the distortion\n"
            "       vec2 DiffTexCoord = normalize(texCoord - WaveCentre);         \n"
            ""
            "        //Perform the distortion and reduce the effect over time\n"
            "       texCoord += ((DiffTexCoord * DiffTime) / (CurrentTime * Dist * 40.0));\n"
            "       Color = texture2D(s_texture, texCoord);\n"
            ""
            "        //Blow out the color and reduce the effect over time\n"
            "       Color += (Color * ScaleDiff) / (CurrentTime * Dist * 40.0);\n"
            "   } \n"
            ""
            "   gl_FragColor = Color; "
            "}";

    programObject = createProgram(vShaderStr, fShaderStr);
    // Bind to attributes
    glBindAttribLocation(programObject, 0, "u_resolution");
    glBindAttribLocation(programObject, 1, "u_time");

    // get indexes to pass to glEnableVertexAttribArray()
    indexResolution = glGetUniformLocation(programObject, "u_resolution");
    indexTime       = glGetUniformLocation(programObject, "u_time");

    // Load the texture
    textureId = loadImage("assets/texture.png");
    if (textureId == 0)
        return false;
    samplerLoc = glGetUniformLocation(programObject, "s_texture");


    // Use the program object
    glUseProgram ( programObject );

    return true;
}

void Shader2::drawFrame(void *args) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat time = (GLfloat) clock() / (GLfloat) CLOCKS_PER_SEC;

    float shaderVertices[] = {
            -1.0F,  -1.0F,  0.0F,
            1.0F,   -1.0F,  0.0F,
            -1.0F,  1.0F,  0.0F,
            1.0F,   -1.0F,  0.0F,
            1.0F,   1.0F,  0.0F,
            -1.0F,   1.0F,  0.0F,
    };
//    float shaderVertices[] = {
//            -1.0F,  -1.0F,  0.0F,
//            -1.0F,   1.0F,  0.0F,
//            1.0F,  -1.0F,  0.0F,
//            -1.0F,   1.0F,  0.0F,
//            1.0F,   1.0F,  0.0F,
//            1.0F,  -1.0F,  0.0F
//    };

    // Load the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, shaderVertices);
    glEnableVertexAttribArray(0);

    float t = time - ((int)(time / 30.0F)) * 30.0F; // pass value from 0 to 30 range
    glUniform1f(indexTime, t);
    glUniform3f(indexResolution, (GLfloat) width, (GLfloat) height, 0.0F);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

