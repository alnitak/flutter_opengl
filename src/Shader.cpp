#include "Shader.h"
#include "common.h"

#include <ctime>
#include <cstring>
#include <random>

#define LOG_TAG_SHADER "NATIVE SHADER"

// Query eglGetError and eventully print it with the [note]
void eglPrintError(const std::string &note) {
    EGLint error = eglGetError();
    std::string ret;
    switch (error) {
        case EGL_SUCCESS:
            ret = "No error";
            break;
        case EGL_NOT_INITIALIZED:
            ret = "EGL not initialized or failed to initialize";
            break;
        case EGL_BAD_ACCESS:
            ret = "Resource inaccessible";
            break;
        case EGL_BAD_ALLOC:
            ret = "Cannot allocate resources";
            break;
        case EGL_BAD_ATTRIBUTE:
            ret = "Unrecognized attribute or attribute value";
            break;
        case EGL_BAD_CONTEXT:
            ret = "Invalid EGL context";
            break;
        case EGL_BAD_CONFIG:
            ret = "Invalid EGL frame buffer configuration";
            break;
        case EGL_BAD_CURRENT_SURFACE:
            ret = "Current surface is no longer valid";
            break;
        case EGL_BAD_DISPLAY:
            ret = "Invalid EGL display";
            break;
        case EGL_BAD_SURFACE:
            ret = "Invalid surface";
            break;
        case EGL_BAD_MATCH:
            ret = "Inconsistent arguments";
            break;
        case EGL_BAD_PARAMETER:
            ret = "Invalid argument";
            break;
        case EGL_BAD_NATIVE_PIXMAP:
            ret = "Invalid native pixmap";
            break;
        case EGL_BAD_NATIVE_WINDOW:
            ret = "Invalid native window";
            break;
        case EGL_CONTEXT_LOST:
            ret = "Context lost";
            break;
        default:
            ret = "Unknown error";
    }
    if (error == EGL_SUCCESS) {
        LOGD(LOG_TAG_SHADER, "%s  %s", note.c_str(), ret.c_str());
    } else {
        LOGD(LOG_TAG_SHADER, "%s  error: %d  0x%X  %s", note.c_str(), error, error, ret.c_str());
    }
}

void replaceAll(std::string& src, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = src.find(search, pos)) != std::string::npos) {
         src.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}

Shader::Shader(OpenglPluginContext *textureStruct)
        : self(textureStruct),
          width(0),
          height(0),
          programObject(0),
          _isContinuous(true),
          uniformsList(UniformQueue()) {
}

Shader::~Shader() {
    if (programObject != 0)
        glDeleteProgram(programObject);
}

// Not yet used. It will be used to draw the scene only when new data to draw it will be sent
void Shader::setIsContinuous(bool isContinuous) {
    _isContinuous = isContinuous;
}

// Adds the basic uniforms used in ShaderToy shaders
// #include "Sampler2D.h"
// Sampler2D sampler = Sampler2D();
void Shader::addShaderToyUniforms() {
    glm::vec4 iMouse = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 iResolution = glm::vec3((GLfloat) width, (GLfloat) height, 0.0f);
    float time = 0.0f;
    uniformsList.addUniform("iMouse", UNIFORM_VEC4, (void *) (&iMouse));
    uniformsList.addUniform("iResolution", UNIFORM_VEC3, (void *) (&iResolution));
    uniformsList.addUniform("iTime", UNIFORM_FLOAT, (void *) (&time));

    // Add black 4x4 texture for each iChannel
    std::vector<unsigned char> rawData(4 * 4 * 4, 0);
    Sampler2D sampler;
    sampler.add_RGBA32(4, 4, rawData.data());
    uniformsList.addUniform("iChannel0", UNIFORM_SAMPLER2D, (void *) (&sampler));
    uniformsList.addUniform("iChannel1", UNIFORM_SAMPLER2D, (void *) (&sampler));
    uniformsList.addUniform("iChannel2", UNIFORM_SAMPLER2D, (void *) (&sampler));
    uniformsList.addUniform("iChannel3", UNIFORM_SAMPLER2D, (void *) (&sampler));
}

void Shader::setShadersSize(int w, int h) {
    width = w;
    height = h;
}

// Set the source of the shaders
void Shader::setShadersText(std::string vertexSource, std::string fragmentSource) {
    this->vertexSource = vertexSource;
    this->fragmentSource = fragmentSource;
    LOGD(LOG_TAG_SHADER, "Shader sources set");
}

std::string Shader::initShader() {
    compileError = "";
    if (programObject != 0)
        glDeleteProgram(programObject);

#ifdef _IS_LINUX_
    gdk_gl_context_make_current(self->context);
#elif _IS_WIN_
    wglMakeCurrent(self->hdc, self->hrc);
#endif

    programObject = createProgram(vertexSource.c_str(), fragmentSource.c_str());
    if (programObject == 0) {
        LOGD(LOG_TAG_SHADER, "Error loading shader");
        return compileError;
    }
    uniformsList.setProgram(programObject);

    startTime = (GLfloat) clock() / (GLfloat) CLOCKS_PER_SEC;

#ifdef _IS_ANDROID_
    // Load the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, shaderVertices);
    glEnableVertexAttribArray(0);
#endif
#if defined _IS_LINUX_ || defined _IS_WIN_
    // use FrameBuffer on Linux
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glViewport(0, 0, width, height);

    glClearColor(.0f, .0f, .0f, 1.f);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(shaderVertices), shaderVertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);
    // Text coords attribute
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
    // glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO

    glBindTexture(GL_TEXTURE_2D, self->texture_name);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glGenFramebuffers(1, (GLuint *)&FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, self->texture_name, 0);
#endif

    uniformsList.setAllSampler2D();

#ifdef _IS_LINUX_
    gdk_gl_context_clear_current();
#elif _IS_WIN_
    // pixelBuffer = make_unique<uint8_t[]>(width * height * 4);
    
    // if (buffer == nullptr) {
    //     LOGD(LOG_TAG_SHADER, "Can't allocate buffer memory!");
    //     if (programObject != 0)
    //         glDeleteProgram(programObject);
    //     compileError = "Can't allocate buffer memory!";
    // }
#endif
    LOGD(LOG_TAG_SHADER, "Shader programmed: %d", programObject);
    return compileError;
}

// Prepare the common vertex shader and uniforms to be used with
// the ShaderToy fragment shader
std::string Shader::initShaderToy() {
    // Shows how to use the mouse input (only left button supported):
    //
    //      mouse.xy  = mouse position during last button down
    //  abs(mouse.zw) = mouse position during last button click
    // sign(mouze.z)  = button is down
    // sign(mouze.w)  = button is clicked
    // https://www.shadertoy.com/view/llySRh
    // https://www.shadertoy.com/view/Mss3zH

    // IMPORT FROM SHADERTOY on Android:
    // Since for now only ES 2 is supported and the below ES 3 functions 
    // are not supported, replace them
    // texture ==> texture2D
    // round == floor
#ifdef _IS_ANDROID_
    replaceAll(fragmentSource, "texture(", "texture2D(");
    replaceAll(fragmentSource, "round(", "floor(");
#endif

    vertexSource =
#ifdef _IS_ANDROID_
//            "#version 300 es\n"
#elif defined _IS_LINUX_ || defined _IS_WIN_
            "#version 330 core\n"
#endif
            "precision highp float;\n"
            "precision mediump int;\n"
            "attribute vec4 a_Position;        \n" // Per-vertex position information we will pass in.

            "void main()                       \n" // The entry point for our vertex shader.
            "{                                 \n"
            "   gl_Position = a_Position;      \n"
            "}                                 \n";

    std::string common = "precision highp float;         \n"
                        // "layout(binding=0) uniform sampler2D iChannel0; \n"
                        // "layout(binding=1) uniform sampler2D iChannel1; \n"
                        // "layout(binding=2) uniform sampler2D iChannel2; \n"
                        // "layout(binding=3) uniform sampler2D iChannel3; \n"
                         "uniform sampler2D iChannel0;   \n"
                         "uniform sampler2D iChannel1;   \n"
                         "uniform sampler2D iChannel2;   \n"
                         "uniform sampler2D iChannel3;   \n"
                         "uniform vec4      iMouse;      \n"  // mouse position (in pixels)
                         "uniform vec3      iResolution; \n"  // viewport resolution (in pixels)
                         "uniform float     iTime;       \n"; // shader playback time (in seconds)

    std::string main = "\nvoid main() {\n"
                       #ifdef _IS_ANDROID_
                       "    mainImage(gl_FragColor, gl_FragCoord.xy);\n"
                       #endif
                       #if defined _IS_LINUX_ || defined _IS_WIN_
                       // The FBO is vertical flipped. It has top-left origin, but OpenGL has bottom-left
                       // So do a flip in vertical axis when using framebuffer
                       "    mainImage(gl_FragColor, vec2(gl_FragCoord.x, iResolution.y-gl_FragCoord.y));\n"
                       #endif
                       "}\n";

    fragmentSource =
#if defined _IS_LINUX_ || defined _IS_WIN_
    "#version 330 core\n"
#endif
    //"#version 300 es\n"
    "#extension GL_OES_standard_derivatives : enable         \n" + // needed by fwidth
    common +
    fragmentSource +
    main;

    addShaderToyUniforms();
    return initShader();
}

GLuint Shader::loadShader(GLenum shaderType, const char *shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &shaderSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char *buf = (char *) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    std::string type = shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT";
                    LOGD(LOG_TAG_SHADER, "Could not Compile %s Shader:\n%s\n", type.c_str(), buf);
                    compileError = type + " shader compile error:\n" + std::string(buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    } else {
        LOGD(LOG_TAG_SHADER,
             "loadShader(), glCreateShader() error: Could not create shader object!!");
    }
    return shader;
}

GLuint Shader::createProgram(const char *vertexSource, const char *fragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader)
        return 0;
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader)
        return 0;

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char *buf = (char *) malloc(sizeof(GLint) * bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGD(LOG_TAG_SHADER, "Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

void Shader::use() const {
    glUseProgram(programObject);
}

// called from the main rendering loop.
// The flow:
// 1 - Make the context current (linux & win)
// 2 - set all uniforms
// 3 - draw into frame buffer object (linux & win). On android draw to texture and swap buffer
// 4 - tell to Flutter texture registrat that a new frame is available
void Shader::drawFrame() {
    if (programObject == 0) return;
    std::lock_guard<std::mutex> lock_guard(mutex_);

#ifdef _IS_LINUX_
    gdk_gl_context_make_current(self->context);
#elif _IS_WIN_
    wglMakeCurrent(self->hdc, self->hrc);
#endif

    glClear(GL_COLOR_BUFFER_BIT);

    GLfloat time = (GLfloat) clock() / (GLfloat) CLOCKS_PER_SEC - startTime;
    use();

    uniformsList.setUniformValue("iTime", (void *) (&time));
    uniformsList.sendAllUniforms();


#ifdef _IS_ANDROID_
    glDrawArrays(GL_TRIANGLES, 0, 6);

    if (!eglSwapBuffers(self->display, self->surface)) {
        eglPrintError("eglSwapBuffers()");
    }
#elif _IS_LINUX_
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    // glBindTexture(GL_TEXTURE_2D, NULL);
    // glFlush();
    // glFinish();

    fl_texture_registrar_mark_texture_frame_available(self->texture_registrar,
                                                      self->texture);
    gdk_gl_context_clear_current();
#elif _IS_WIN_
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, NULL);

    glBindFramebuffer(GLenum(GL_FRAMEBUFFER), FBO);
    /// uff... takes me ages to understand that Widnwos doesn't like glGetTexImage??*@##[]][!!
    // glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)(self->myTexture->buffer.data()));
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, (void*)(self->myTexture->buffer.data()));

    self->myTexture->Update();
#endif
}
