// Copyright 2019 Marco Bavagnoli <marco.bavagnoli@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#ifndef RENDERER_H
#define RENDERER_H

#include <pthread.h>
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES2/gl2.h>
#include <GLES/gl.h>
#include <GLES2/gl2ext.h>
#include <jni.h>
#include <memory>

#include "Shader.h"


enum CurrDrawingFunc {
    FUNC_DEFAULT = 0,
    FUNC_WAVES = 1,
    FUNC_WAVES2 = 2,
    FUNC_WAVES3 = 3,
};

struct RendererDataTime {
    GLfloat time;
};


void setOrtho (GLfloat *m, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far);


bool initShader(CurrDrawingFunc currDrawingFunc);



class Renderer {

public:

    Renderer(int ES_version,
             Shader *shader,
             bool continuous,
             const char *name,
             int width, int height,
             float scaleX, float scaleY,
             int clearR, int clearG, int clearB, int clearA);
    virtual ~Renderer();

    // Following methods can be called from any thread.
    // They send message to render thread which executes required actions.
    void start();
    void stop();
    void clear();
    void setNativeWindow(JNIEnv* jenv, jobject surface);
//    void setWindow(ANativeWindow* window);

    void destroy();

    int getWindowWidth();
    int getWindowHeight();

    bool isFrameDrawing();

    int processNewData(void *args);

    int isContextValid();
    bool isLoopingRunning();
    bool isThreadRunning();


    void setClearColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);

private:

    enum RenderThreadMessage {
        MSG_NONE = 0,
        MSG_WINDOW_SET,
        MSG_RENDER_LOOP_EXIT_AND_DESTROY,
        MSG_RENDER_LOOP_EXIT,
        MSG_DATA_RECEIVED,   // when new data is ready, the draw function is called which must then free its memory
        MSG_RENDER_CLEAR_VIEW,
        MSG_INIT_SHADER
    };

    pthread_t _threadId;
    pthread_mutex_t _mutexRenderer;
    enum RenderThreadMessage _msg;
    bool loopRunning;
    bool threadRunning;
    char debugName[50];

    // android window, supported by NDK r5 and newer
    ANativeWindow* _window;

    EGLDisplay _display;
    EGLSurface _surface;
    EGLContext _context;

//    Renderer::getWindowWidth() and Renderer::getWindowHeight() doesn't work correctly:
//    they are used in `Renderer::initializeGL()` to set window buffer size with ANativeWindow_setBuffersGeometry(),
//    without it the window `surface` is not working and nothing is displayed.
    int _width;
    int _height;

    float bufferScaleX;
    float bufferScaleY;

    bool isContinuous;

    Shader *shader;
    void *_args;
    bool isDrawing;


    // RenderLoop is called in a rendering thread started in start() method
    // It creates rendering context and renders scene until stop() is called
    void renderLoop();
    
    bool initializeGL();

    // Helper method for starting the thread
    static void* threadStartCallback(void *myself);

};

#endif // RENDERER_H

