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


#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <sys/resource.h> // for setpriority

#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h>

#include <malloc.h>
#include <math.h>
#include <ios>
#include <android/log.h>

#include "ndk/common.hpp"
#include "Shader.h"
#include "renderer.h"

#define LOG_TAG_RENDERER "native renderer"

#define DEBUG true

int RENDERER_EGL_VERSION=2;



float clearColorR;
float clearColorG;
float clearColorB;
float clearColorA;

/**
 *
 * @param ES_version
 * @param shaderFunc OpenGL initializer function for the drawing function
 * @param drawFunc OpenGL draw frame function
 * @param continuous if true the `drawFunc` is called each render loop.
 *        If false `drawFunc` is called only when new data is received by
 *        calling `processNewData(void *args)`
 * @param name for debug purpose
 * @param width
 * @param height real window size.
 *        Renderer::getWindowWidth() and Renderer::getWindowHeight() doesn't work correctly:
 *        they are used in `Renderer::initializeGL()` to set window buffer size with
 *        `ANativeWindow_setBuffersGeometry()`, without it the window `surface` is not working
 *        and nothing is displayed.
 * @param scaleX
 * @param scaleY window buffer scale. Used for example when displaying a full screen window to improve performances
 * @param clearR
 * @param clearG
 * @param clearB
 * @param clearA clear color
 */
Renderer::Renderer(int ES_version,
                   Shader *shader,
                   bool continuous,
                   const char *name,
                   int width, int height,
                   float scaleX, float scaleY,
                   int clearR, int clearG, int clearB, int clearA)
    : _msg(MSG_NONE), _display(0), _surface(0), _context(0), _window(0), _args(0)
{
    if (DEBUG) LOGI(LOG_TAG_RENDERER, "Renderer instance created");
    pthread_mutex_init(&_mutexRenderer, 0);
    RENDERER_EGL_VERSION = ES_version;
    loopRunning = false;
    threadRunning = false;
    _threadId = 0;
    _width = width;
    _height = height;
    bufferScaleX = scaleX;
    bufferScaleY = scaleY;
    isContinuous = continuous;
    strcpy(debugName, name);

    this->shader = shader;
//    initShader=&Shader::initShader;
//    drawFrame=&Shader::drawFrame;
//    setFunctions( initShader, drawFrame );

    setClearColor((const unsigned char)clearR,(const unsigned char)clearG,(const unsigned char)clearB,(const unsigned char)clearA);
}

Renderer::~Renderer()
{
    if (this == NULL) return;
    if (DEBUG) LOGI(LOG_TAG_RENDERER, "~Renderer ~instance ~destroyed");
    while (isLoopingRunning());
    pthread_mutex_lock(&_mutexRenderer);
    _window = NULL;
    pthread_mutex_unlock(&_mutexRenderer);
    pthread_mutex_destroy(&_mutexRenderer);
}


void Renderer::setNativeWindow(JNIEnv* jenv, jobject surface)
{
    if (this == NULL) return;
    pthread_mutex_lock(&_mutexRenderer);
    if (_window!=NULL) {
        if (DEBUG) LOGI(LOG_TAG_RENDERER, "setNativeWindow() stopping");
        pthread_mutex_unlock(&_mutexRenderer);
        stop();
        pthread_mutex_lock(&_mutexRenderer);
    }
    _window = ANativeWindow_fromSurface(jenv, surface);
    pthread_mutex_unlock(&_mutexRenderer);
    if (DEBUG) LOGI(LOG_TAG_RENDERER, "setNativeWindow() Got window %p", _window);
}

void Renderer::start()
{
    if (this == NULL) return;
    if (threadRunning) return;
    if (_window == NULL) return;
    pthread_mutex_lock(&_mutexRenderer);

    if (DEBUG) LOGI(LOG_TAG_RENDERER, "start() Renderer thread start - Creating renderer thread");


//    pthread_attr_t tattr;
//    int ret;
//    int newprio = 19; // -20 highest   19 lowest
//    sched_param param;
//
//    /* initialized with default attributes */
//    ret = pthread_attr_init (&tattr);
//
//    /* safe to get existing scheduling param */
//    ret = pthread_attr_getschedparam (&tattr, &param);
//
//    /* set the priority; others are unchanged */
//    param.sched_priority = newprio;
////    param.sched_priority = 10;
//
//    /* setting the new scheduling param */
//    ret = pthread_attr_setschedparam (&tattr, &param);
//
//    /* with new priority specified */
//    ret = pthread_create (&_threadId, &tattr, threadStartCallback, this);

//    int min;
//    int max;
//    min = sched_get_priority_min(SCHED_FIFO);
//    max = sched_get_priority_max(SCHED_FIFO);

//    setpriority();
//    nice(+10);


    int ret = pthread_create(&_threadId, 0, threadStartCallback, this);
    if ( ret != 0) {
        if (DEBUG) LOGI(LOG_TAG_RENDERER, "start() Failed to create thread: %d", ret);
        return;
    } else {
        if (DEBUG) LOGI(LOG_TAG_RENDERER, "start() Renderer thread started");
        if ( ! pthread_detach(_threadId) ) {
            if (DEBUG) LOGI(LOG_TAG_RENDERER, "start() Thread detached successfully !!!\n");
        }
    }

    _msg = MSG_WINDOW_SET;
    pthread_mutex_unlock(&_mutexRenderer);

}

void Renderer::stop()
{
    if (this == NULL) return;
//    if (_window == NULL) return;
    if (DEBUG) LOGI(LOG_TAG_RENDERER, "stop() Stopping");

    if (threadRunning) {
        loopRunning = false;
        struct timespec wait;
        int ret;
        clock_gettime(CLOCK_REALTIME, &wait);
        wait.tv_sec++;
        if (DEBUG) LOGI(LOG_TAG_RENDERER, "stop() Stopping2  %ld", wait.tv_sec);
        ret = pthread_mutex_timedlock(&_mutexRenderer, &wait);
        if (DEBUG) LOGI(LOG_TAG_RENDERER, "stop() Stopping3");
        if (ret == 0) { // ret!=0  = timed out
            if (DEBUG) LOGI(LOG_TAG_RENDERER, "stop() Stopping4 %ld", _threadId);
//            if (threadRunning)
    //            pthread_join(_threadId, 0);
                pthread_kill(_threadId, 0);
            if (DEBUG) LOGI(LOG_TAG_RENDERER, "stop() Stopping5");
            _threadId = 0;
        }
        pthread_mutex_unlock(&_mutexRenderer);
        if (DEBUG) LOGI(LOG_TAG_RENDERER, "stop() Stopping  RET=%d", ret);
    }

    if (DEBUG) LOGI(LOG_TAG_RENDERER, "stop() Stopped");
}


void Renderer::destroy() {
    if (this == NULL) return;
    if (DEBUG) LOGD(LOG_TAG_RENDERER, "destroy()");
    if (_display != EGL_NO_DISPLAY)
        eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (_display != EGL_NO_DISPLAY && _context != EGL_NO_CONTEXT)
        eglDestroyContext(_display, _context);
    if (_display != EGL_NO_DISPLAY && _surface != EGL_NO_SURFACE)
        eglDestroySurface(_display, _surface);
    if (_display != EGL_NO_DISPLAY)
        eglTerminate(_display);

    _display = EGL_NO_DISPLAY;
    _surface = EGL_NO_SURFACE;
    _context = EGL_NO_CONTEXT;

    if (_window != NULL) {
        if (DEBUG) LOGD(LOG_TAG_RENDERER, "WINDOW  %p", _window);
        ANativeWindow_release(_window);
    }
    _window = NULL;

    if (DEBUG) LOGD(LOG_TAG_RENDERER, "destroyED");
}


void Renderer::clear()
{
    if (this == NULL) return;
    if (_window == NULL) return;
    if (DEBUG) LOGI(LOG_TAG_RENDERER, "clear()");

    // send message to render thread to clear view with the given clear color
    pthread_mutex_lock(&_mutexRenderer);
    _msg = MSG_RENDER_CLEAR_VIEW;
    pthread_mutex_unlock(&_mutexRenderer);
}

int Renderer::isContextValid() {
    if (this == NULL) return -1;
    return _context != EGL_NO_CONTEXT;
}

bool Renderer::isLoopingRunning()
{
    if (this == NULL) return false;
    return loopRunning;
}

bool Renderer::isThreadRunning()
{
    if (this == NULL) return false;
    return threadRunning;
}



// TODO these functions return 1 for some reasons I don't know.
// TODO These sizes are written in initializeGL() with the given Flutter Texture() widget size for now
int Renderer::getWindowWidth()
{
    if (this == NULL) return 0;
    return ANativeWindow_getWidth(_window);;
}

int Renderer::getWindowHeight()
{
    if (this == NULL) return 0;
    return ANativeWindow_getHeight(_window);
}


// return 0 if args need to be deleted
int Renderer::processNewData(void *args) {

    if (this == NULL || isDrawing) return 0;
    if (_window == NULL || _msg == MSG_WINDOW_SET) {
        return 0;
    }

    // send message to render thread
    if (args != NULL) {
//        pthread_mutex_lock(&_mutexRenderer);
        _args = args;
        _msg = MSG_DATA_RECEIVED;
//        pthread_mutex_unlock(&_mutexRenderer);
    }
    return 1;
}


bool Renderer::isFrameDrawing()
{
    if (this == NULL) return false;
    return isDrawing;
}

//////////////////////////////////
// RENDERLOOP
//////////////////////////////////
void Renderer::renderLoop()
{
    if (this == NULL) return;
    if (_window == NULL) return;
    if (loopRunning) return;
    loopRunning = true;
    isDrawing = false;

    if (DEBUG) LOGI(LOG_TAG_RENDERER, "LOOP ENTER");

    while (loopRunning) {

        pthread_mutex_lock(&_mutexRenderer);
        if (this == NULL) {
            pthread_mutex_unlock(&_mutexRenderer);
            return;
        }
        // process incoming messages
        switch (_msg) {

            case MSG_WINDOW_SET:
                _msg = MSG_NONE;
                if (DEBUG) LOGI(LOG_TAG_RENDERER, "MSG_WINDOW_SET Initializing context");
                isDrawing = true;
                initializeGL();
                isDrawing = false;
                if (DEBUG) LOGI(LOG_TAG_RENDERER, "MSG_WINDOW_SET InitializED context");
                break;

            case MSG_RENDER_LOOP_EXIT_AND_DESTROY:
                _msg = MSG_NONE;
                if (DEBUG) LOGI(LOG_TAG_RENDERER, "MSG_RENDER_LOOP_EXIT_AND_DESTROY Destroying context");
                destroy();
                loopRunning = false;
                if (DEBUG) LOGI(LOG_TAG_RENDERER, "MSG_RENDER_LOOP_EXIT_AND_DESTROY Destroyed context");
                break;

            case MSG_INIT_SHADER:
                _msg = MSG_NONE;
                if (!shader->initShader(NULL)) {
                    if (DEBUG)
                        LOGE(LOG_TAG_RENDERER, "MSG_INIT_SHADER Cannot initialize shader  context: %d  window: %d", _context, _window);

                    loopRunning = false;
                    pthread_join(_threadId, 0);
                    _threadId = 0;
                    stop();
                    break;
                } else
                    if (DEBUG) LOGI(LOG_TAG_RENDERER, "MSG_INIT_SHADER ok");
                break;

            case MSG_RENDER_CLEAR_VIEW:
                _msg = MSG_NONE;
                if (DEBUG) LOGI(LOG_TAG_RENDERER, " MSG_RENDER_CLEAR_VIEWLOOP clear view");
                glClearColor(clearColorR, clearColorG, clearColorB, clearColorA);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                if (!eglSwapBuffers(_display, _surface)) {
                    LOGE(LOG_TAG_RENDERER, "eglSwapBuffers() returned error %d", eglGetError());
                }
                break;


            case MSG_RENDER_LOOP_EXIT:
                _msg = MSG_NONE;
                if (DEBUG) LOGI(LOG_TAG_RENDERER, "MSG_RENDER_LOOP_EXIT LOOP exiting");
                loopRunning = false;
                break;

            case MSG_DATA_RECEIVED:
                _msg = MSG_NONE;
                if (_display && !isDrawing  && _args!=NULL) {
                    isDrawing=true;
                    shader->drawFrame(_args);
                    if (!eglSwapBuffers(_display, _surface)) {
                        Shader::eglPrintError("eglSwapBuffers()", eglGetError());
                    }
                    _args = NULL;
                    isDrawing=false;
                }
                break;

            default:
                if (isContinuous) {
                    if (_display && !isDrawing) {
                        isDrawing = true;
                        shader->drawFrame(_args);
                        if (!eglSwapBuffers(_display, _surface)) {
                            Shader::eglPrintError("eglSwapBuffers()", eglGetError());
                        }
                        _args = NULL;
                        isDrawing = false;
                    }
                }
                break;
        }

        pthread_mutex_unlock(&_mutexRenderer);
    }
    if (DEBUG) LOGI(LOG_TAG_RENDERER, "LOOP EXITED");
}

bool Renderer::initializeGL()
{
    if (this == NULL) return false;
    if (_window == NULL) return false;
    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_NONE
    };
    EGLConfig config;
    EGLint numConfigs;
    EGLint format;
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
    GLfloat ratio;
    GLsizei width, height;


    if (DEBUG)
        LOGI(LOG_TAG_RENDERER, "****************initializeGL()");

    if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
        Shader::eglPrintError("eglGetDisplay()", eglGetError());
        return false;
    }
    if (!eglInitialize(display, 0, 0)) {
        Shader::eglPrintError("eglInitialize()", eglGetError());
        return false;
    }

    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
        Shader::eglPrintError("eglChooseConfig()", eglGetError());
        destroy();
        return false;
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        Shader::eglPrintError("eglGetConfigAttrib()", eglGetError());
        destroy();
        return false;
    }

    // TODO ANativeWindow_getWidth doesn't get real value
//    int mBufferWidth  = (int)(ANativeWindow_getWidth(_window) * bufferScaleX);
//    int mBufferHeight = (int)(ANativeWindow_getHeight(_window) * bufferScaleY);
    int mBufferWidth  = (int)(_width * bufferScaleX);
    int mBufferHeight = (int)(_height * bufferScaleY);
    ANativeWindow_setBuffersGeometry(_window, mBufferWidth, mBufferHeight, format);

    if (!(surface = eglCreateWindowSurface(display, config, _window, 0))) {
        Shader::eglPrintError("eglCreateWindowSurface()", eglGetError());
        destroy();
        return false;
    }

    const EGLint attribs2[] = {
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE,       8,
            EGL_GREEN_SIZE,      8,
            EGL_RED_SIZE,        8,
            EGL_NONE
    };
    eglChooseConfig(display, attribs2, &config, 1, &numConfigs);
    const EGLint context_attribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, RENDERER_EGL_VERSION, EGL_NONE
    };
    if (!(context = eglCreateContext(display, config, 0, context_attribs))) {
        Shader::eglPrintError("eglCreateContext()", eglGetError());
        destroy();
        return false;
    }

    if (!eglMakeCurrent(display, surface, surface, context)) {
        Shader::eglPrintError("eglMakeCurrent()", eglGetError());
        destroy();
        return false;
    }

    if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
        !eglQuerySurface(display, surface, EGL_HEIGHT, &height)) {
        Shader::eglPrintError("eglQuerySurface()", eglGetError());
        destroy();
        return false;
    }

    _display = display;
    _surface = surface;
    _context = context;

//    if (!shader->initShader(NULL)) {
//        Shader::eglPrintError("initShader()", eglGetError());
//        destroy();
//        return false;
//    }

    if (DEBUG)
        LOGI(LOG_TAG_RENDERER, "initializeGL() context: %d windows: %p   %dx%d",
                    _context, _window, width, height);


    glClearColor(clearColorR, clearColorG, clearColorB, clearColorA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // swap to clear window
    if (!eglSwapBuffers(_display, _surface)) {
        Shader::eglPrintError("eglSwapBuffers()", eglGetError());
    }

    _msg = MSG_INIT_SHADER;

    return true;
}


void* Renderer::threadStartCallback(void *myself)
{
    Renderer *renderer = (Renderer*)myself;

    renderer->threadRunning = true;
    renderer->renderLoop();
    renderer->threadRunning = false;
    renderer->_threadId = 0;

    pthread_exit(0);
}



/////////////////////////////
/// UTILS
/////////////////////////////
void Renderer::setClearColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a) {
    pthread_mutex_lock(&_mutexRenderer);
    clearColorR=(float)r/255.0f;
    clearColorG=(float)g/255.0f;
    clearColorB=(float)b/255.0f;
    clearColorA=(float)a/255.0f;
    pthread_mutex_unlock(&_mutexRenderer);
}





