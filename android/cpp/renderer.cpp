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
#include <string>
#include <math.h>
#include <ios>

#include "common.hpp"
#include "shaders.h"
#include "renderer.h"

#define LOG_TAG_RENDERER "renderer"

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
                   bool (*shaderFunc)(void *args),
                   void (*drawFunc)(void *args),
                   bool continuous,
                   const char *name,
                   int width, int height,
                   float scaleX, float scaleY,
                   int clearR, int clearG, int clearB, int clearA)
    : _msg(MSG_NONE), _display(0), _surface(0), _context(0), _window(0), _args(0)
{
    if (DEBUG) LOGI("Renderer instance created");
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

    setFunctions(shaderFunc, drawFunc);
    setClearColor((const unsigned char)clearR,(const unsigned char)clearG,(const unsigned char)clearB,(const unsigned char)clearA);
}

Renderer::~Renderer()
{
    if (this == NULL) return;
    if (DEBUG) LOGI("~Renderer ~instance ~destroyed");
    while (isLoopingRunning());
    pthread_mutex_lock(&_mutexRenderer);
    _window = NULL;
    glDeleteProgram(getShadersCommonParams().programObject);
    pthread_mutex_unlock(&_mutexRenderer);
    pthread_mutex_destroy(&_mutexRenderer);
}


void Renderer::setNativeWindow(JNIEnv* jenv, jobject surface)
{
    if (this == NULL) return;
    pthread_mutex_lock(&_mutexRenderer);
    if (_window!=NULL) {
        if (DEBUG) LOGI("setNativeWindow() stopping");
        pthread_mutex_unlock(&_mutexRenderer);
        stop();
        pthread_mutex_lock(&_mutexRenderer);
    }
    _window = ANativeWindow_fromSurface(jenv, surface);
    pthread_mutex_unlock(&_mutexRenderer);
    if (DEBUG) LOGI("setNativeWindow() Got window %p", _window);
}

void Renderer::start()
{
    if (this == NULL) return;
    pthread_mutex_lock(&_mutexRenderer);
    if (threadRunning) return;
    if (_window == NULL) return;

    if (DEBUG) LOGI("start() Renderer thread start - Creating renderer thread");


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
////
//    /* with new priority specified */
//    ret = pthread_create (&_threadId, &tattr, threadStartCallback, this);

//    int min;
//    int max;
//    min = sched_get_priority_min(SCHED_FIFO);
//    max = sched_get_priority_max(SCHED_FIFO);

//    setpriority();
//    nice(+10);

    _msg = MSG_WINDOW_SET;
    pthread_mutex_unlock(&_mutexRenderer);

    int ret = pthread_create(&_threadId, 0, threadStartCallback, this);
    if ( ret != 0) {
        if (DEBUG) LOGI("start() Failed to create thread: %d", ret);
        return;
    } else {
        if (DEBUG) LOGI("start() Renderer thread started");
        if ( ! pthread_detach(_threadId) ) {
            if (DEBUG) LOGI("start() Thread detached successfully !!!\n");
        }
    }

}

void Renderer::stop()
{
    if (this == NULL) return;
//    if (_window == NULL) return;
    if (DEBUG) LOGI("stop() Stopping");

    if (threadRunning) {
        loopRunning = false;
        struct timespec wait;
        int ret;
        clock_gettime(CLOCK_REALTIME, &wait);
        wait.tv_sec++;
        if (DEBUG) LOGI("stop() Stopping2  %ld", wait.tv_sec);
        ret = pthread_mutex_timedlock(&_mutexRenderer, &wait);
        if (DEBUG) LOGI("stop() Stopping3");
        if (ret == 0) { // ret!=0  = timed out
            if (DEBUG) LOGI("stop() Stopping4 %ld", _threadId);
//            if (threadRunning)
    //            pthread_join(_threadId, 0);
                pthread_kill(_threadId, 0);
            if (DEBUG) LOGI("stop() Stopping5");
            _threadId = 0;
        }
        pthread_mutex_unlock(&_mutexRenderer);
        if (DEBUG) LOGI("stop() Stopping  RET=%d", ret);
    }

    if (DEBUG) LOGI("stop() Stopped");
}


void Renderer::destroy() {
    if (this == NULL) return;
    if (DEBUG) LOGD("destroy()");
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
        if (DEBUG) LOGD("WINDOW  %p", _window);
        ANativeWindow_release(_window);
    }
    _window = NULL;

    if (DEBUG) LOGD("destroyED");
}


void Renderer::clear()
{
    if (this == NULL) return;
    if (_window == NULL) return;
    if (DEBUG) LOGI("clear()");

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



void Renderer::setFunctions(bool (*shaderFunc)(void *args),
                            void (*drawFunc)(void *args)) {
    if (this == NULL) return;
    if (threadRunning && loopRunning) {
        pthread_mutex_lock(&_mutexRenderer);
        if (this->drawFrame != drawFrame) {
            this->drawFrame = drawFrame;
            this->initShaderFunc = initShaderFunc;
            _msg = MSG_INIT_SHADER;
        }
        pthread_mutex_unlock(&_mutexRenderer);
    } else {
        this->drawFrame = drawFunc;
        this->initShaderFunc = shaderFunc;
    }
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

    if (DEBUG) LOGI("LOOP ENTER");

    while (loopRunning) {

        pthread_mutex_lock(&_mutexRenderer);
        if (this == NULL) {
            pthread_mutex_unlock(&_mutexRenderer);
            return;
        }
        // process incoming messages
        switch (_msg) {

            case MSG_WINDOW_SET:
                if (DEBUG) LOGI("Initializing context");
                isDrawing = true;
                initializeGL();
                isDrawing = false;
                if (DEBUG) LOGI("InitializED context");
                break;

            case MSG_RENDER_LOOP_EXIT_AND_DESTROY:
                if (DEBUG) LOGI("Destroying context");
                destroy();
                loopRunning = false;
                if (DEBUG) LOGI("Destroyed context");
                break;

            case MSG_INIT_SHADER:
                if (!initShaderFunc(NULL)) {
                    if (DEBUG) LOGE("Cannot initialize shader  context: %d  window: %d", _context, _window);
                    loopRunning = false;
                    pthread_join(_threadId, 0);
                    _threadId = 0;
//                    destroy();
//                    delete this;

//                    if (_context == NULL) {
//                        LOGE("DESTROYING & reINITIALIZING");
//                        destroy();
//                        initializeGL();
//                    }
                    stop();
                    break;
                }
                break;

            case MSG_RENDER_CLEAR_VIEW:
                if (DEBUG) LOGI("LOOP clear view");
                glClearColor(clearColorR, clearColorG, clearColorB, clearColorA);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                if (!eglSwapBuffers(_display, _surface)) {
                    LOGE("eglSwapBuffers() returned error %d", eglGetError());
                }
                break;


            case MSG_RENDER_LOOP_EXIT:
                if (DEBUG) LOGI("LOOP exiting");
                loopRunning = false;
                break;

            case MSG_DATA_RECEIVED:

                if (_display && !isDrawing && drawFrame!=NULL && _args!=NULL) {
                    isDrawing=true;
                    (*drawFrame)(_args);
                    if (!eglSwapBuffers(_display, _surface)) {
                        eglPrintError("eglSwapBuffers()", eglGetError());
                    }
                    _args = NULL;
                    isDrawing=false;
                }
                break;

            default:
                if (isContinuous) {
                    if (_display && !isDrawing && drawFrame != NULL) {
                        isDrawing = true;
                        (*drawFrame)(_args);
                        if (!eglSwapBuffers(_display, _surface)) {
                            eglPrintError("eglSwapBuffers()", eglGetError());
                        }
                        _args = NULL;
                        isDrawing = false;
                    }
                }
                break;
        }
        _msg = MSG_NONE;

        pthread_mutex_unlock(&_mutexRenderer);
    }
    if (DEBUG) LOGI("LOOP EXITED");
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
        LOGI("****************initializeGL()");

    if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
        eglPrintError("eglGetDisplay()", eglGetError());
        return false;
    }
    if (!eglInitialize(display, 0, 0)) {
        eglPrintError("eglInitialize()", eglGetError());
        return false;
    }

    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
        eglPrintError("eglChooseConfig()", eglGetError());
        destroy();
        return false;
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        eglPrintError("eglGetConfigAttrib()", eglGetError());
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
        eglPrintError("eglCreateWindowSurface()", eglGetError());
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
        eglPrintError("eglCreateContext()", eglGetError());
        destroy();
        return false;
    }

    if (!eglMakeCurrent(display, surface, surface, context)) {
        eglPrintError("eglMakeCurrent()", eglGetError());
        destroy();
        return false;
    }

    if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
        !eglQuerySurface(display, surface, EGL_HEIGHT, &height)) {
        eglPrintError("eglQuerySurface()", eglGetError());
        destroy();
        return false;
    }

    _display = display;
    _surface = surface;
    _context = context;

    setShadersCommonParamsWidth(width);
    setShadersCommonParamsHeight(height);

    if (!(initShaderFunc)(NULL)) {
        eglPrintError("initShader()", eglGetError());
        destroy();
        return false;
    }

    if (DEBUG)
        LOGI("initializeGL() context: %d windows: %p  %d   %dx%d",
                    _context, _window, initShaderFunc, width, height);


    glClearColor(clearColorR, clearColorG, clearColorB, clearColorA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    glViewport(0, 0, width, height);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();



    // to get clear to be set
    if (!eglSwapBuffers(_display, _surface)) {
        eglPrintError("eglSwapBuffers()", eglGetError());
    }

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




///////////////////////////////////
/// OpenGL ES 2
///////////////////////////////////
void setOrtho(GLfloat *m, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near,
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











