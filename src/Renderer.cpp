#include "Renderer.h"

#ifdef _IS_ANDROID_

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/surface_texture.h>
#include <android/surface_texture_jni.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#elif _IS_LINUX_
    #include "uniformQueue.h"
#elif _IS_WIN_
    #include <GL/glew.h>
#endif


#define LOG_TAG_RENDERER "RENDERER"
#define DEBUG true

Renderer::Renderer(OpenglPluginContext *textureStruct)
        : self(textureStruct),
          frameRate(0.0),
          camera(nullptr),
          shader(new Shader(textureStruct)),
          isShaderToy(false),
          loopRunning(false)
{
    msg.push_back(MSG_NONE);
}

Renderer::~Renderer() {
    if (camera != nullptr) stopCamera();
   
    if (shader.get() != nullptr) {
        shader.reset();
        shader.release();
    }

#ifdef _IS_WIN_
    if (self->hrc) {
        wglMakeCurrent( NULL, NULL );
        wglDeleteContext(self->hrc); 
    }

    if (self->hdc) 
        ReleaseDC(self->m_hWnd, self->hdc);

    if (self->myTexture != nullptr) {
        delete self->myTexture;
        self->myTexture = nullptr;
    }
#endif
}

#ifdef _IS_ANDROID_

// TODO: try to use FBO in Android
bool Renderer::setNativeAndroidWindow(jobject surface) {
    self->window = ANativeWindow_fromSurface(self->g_env, surface);
    self->surface = surface;
    return true;
}

bool Renderer::initOpenGL() {
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
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
    EGLint majorVersion, minorVersion;

    if (DEBUG)
        LOGD(LOG_TAG_RENDERER, "****************initializeGL()");

    if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
        eglPrintError("eglGetDisplay()");
        return false;
    }
    if (!eglInitialize(display, &majorVersion, &minorVersion)) {
        eglPrintError("eglInitialize()");
        return false;
    }

    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
        eglPrintError("eglChooseConfig()");
        return false;
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        eglPrintError("eglGetConfigAttrib()");
        return false;
    }

    // TODO ANativeWindow_getWidth doesn't get real value
//    int mBufferWidth  = (int)(ANativeWindow_getWidth(_window) * bufferScaleX);
//    int mBufferHeight = (int)(ANativeWindow_getHeight(_window) * bufferScaleY);
    int mBufferWidth = (int) (self->width);
    int mBufferHeight = (int) (self->height);
    ANativeWindow_setBuffersGeometry(self->window, mBufferWidth, mBufferHeight, format);

    if (!(surface = eglCreateWindowSurface(display, config, self->window, 0))) {
        eglPrintError("eglCreateWindowSurface()");
        return false;
    }

    // can't get ES3 to work!
    const EGLint attribs2[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    eglChooseConfig(display, attribs2, &config, 1, &numConfigs);
    const EGLint context_attribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
    };
    if (!(context = eglCreateContext(display, config, 0, context_attribs))) {
        eglPrintError("eglCreateContext()");
        return false;
    }

    if (!eglMakeCurrent(display, surface, surface, context)) {
        eglPrintError("eglMakeCurrent()");
        return false;
    }

    if (!eglQuerySurface(display, surface, EGL_WIDTH, &self->width) ||
        !eglQuerySurface(display, surface, EGL_HEIGHT, &self->height)) {
        eglPrintError("eglQuerySurface()");
        return false;
    }

    eglSwapInterval(self->display, 1);

    self->display = display;
    self->surface = surface;
    self->context = context;

    return true;
}

void Renderer::destroyGL() {
    if (DEBUG) LOGD(LOG_TAG_RENDERER, "destroy()");
    if (self->display != EGL_NO_DISPLAY)
        eglMakeCurrent(self->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (self->display != EGL_NO_DISPLAY && self->context != EGL_NO_CONTEXT)
        eglDestroyContext(self->display, self->context);
    if (self->display != EGL_NO_DISPLAY && self->surface != EGL_NO_SURFACE)
        eglDestroySurface(self->display, self->surface);
    if (self->display != EGL_NO_DISPLAY)
        eglTerminate(self->display);

    self->display = EGL_NO_DISPLAY;
    self->surface = EGL_NO_SURFACE;
    self->context = EGL_NO_CONTEXT;

    if (self->window != nullptr) {
        // native_window_api_disconnect(_window, NATIVE_WINDOW_API_EGL);
        ANativeWindow_release(self->window);
    }
    self->window = nullptr;

    if (DEBUG) LOGD(LOG_TAG_RENDERER, "destroyED");
}

#endif
#ifdef _IS_WIN_
GLint Renderer::setPixelFormat()
{
   static PIXELFORMATDESCRIPTOR pfd=    
    {
     sizeof(PIXELFORMATDESCRIPTOR),   
     1,          
     PFD_DRAW_TO_WINDOW |     
     PFD_SUPPORT_OPENGL |     
     PFD_DOUBLEBUFFER,      
     PFD_TYPE_RGBA,       
     16,          
     0, 0, 0, 0, 0, 0,      
     0,          
     0,          
     0,          
     0, 0, 0, 0,        
     16,          
     0,          
     0,          
     PFD_MAIN_PLANE,       
     0,          
     0, 0, 0         
    };

   GLint  iPixelFormat; 

   // get the device context's best, available pixel format match 
   if((iPixelFormat = ChoosePixelFormat(self->hdc, &pfd)) == 0)
   {
    LOGD(LOG_TAG_RENDERER, "ChoosePixelFormat Failed");
    return 0;
   }


   // make that match the device context's current pixel format 
   if(SetPixelFormat(self->hdc, iPixelFormat, &pfd) == FALSE)
   {
    LOGD(LOG_TAG_RENDERER, "SetPixelFormat Failed");
    return 0;
   }

   if((self->hrc = wglCreateContext(self->hdc)) == NULL)
   {
    LOGD(LOG_TAG_RENDERER, "wglCreateContext Failed");
    return 0;
   }

   if((wglMakeCurrent(self->hdc, self->hrc)) == NULL)
   {
    LOGD(LOG_TAG_RENDERER, "wglMakeCurrent Failed");
    return 0;
   }


   return 1;
}
bool Renderer::initOpenGL()
{
    /* Get the handle of the windows device context. */ 
    self->hdc = GetDC(self->m_hWnd); 
 
    bool b;
    b = wglMakeCurrent(self->hdc, NULL);
    b = setPixelFormat();

    // Initialize GL
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
      /* Problem: glewInit failed, something is seriously wrong. */
      LOGD(LOG_TAG_RENDERER, "Error: %s", glewGetErrorString(err));
      msg.push_back(MSG_STOP_RENDERER);
      return false;
    }

    glGenTextures(1, &self->texture_name);
    glBindTexture(GL_TEXTURE_2D, self->texture_name); 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, self->width, self->height, 
            0, GL_BGR, GL_UNSIGNED_BYTE, nullptr);
    return true;
}
void Renderer::destroyGL() 
{
}
#endif

OpenCVCamera *Renderer::getOpenCVCamera() 
{ 
    return camera; 
}

bool Renderer::openCamera(std::string uniformName, int width, int height)
{
    if (camera != nullptr) return false;
    camera = new OpenCVCamera();
    bool opened = camera->open(uniformName, width, height);
    if (!opened)
    {
        delete camera;
        camera = nullptr;
        return false;
    }
    return true;
}

bool Renderer::stopCamera()
{
    if (camera == nullptr) return false;
    delete camera;
    camera = nullptr;
    LOGD(LOG_TAG_RENDERER, "CAMERA STOPPED");
    return true;
}

void Renderer::stop() {
    msg.push_back(MSG_STOP_RENDERER);
}

// Set the message for the main loop that a new shader should be used
std::string Renderer::setShader(bool isContinuous,
                                const char *vertexSource,
                                const char *fragmentSource) {
    compileError = "";
    isShaderToy = false;

    newShaderFragmentSource = fragmentSource;
    newShaderVertexSource = vertexSource;
    newShaderIsContinuous = isContinuous;
    msg.push_back(MSG_NEW_SHADER);
    if (loopRunning)
        while (msg.back() == MSG_NEW_SHADER);
    return compileError;
}

// Set the message for the main loop that a new ShaderToy shader should be used
std::string Renderer::setShaderToy(const char *fragmentSource) {
    compileError = "";
    isShaderToy = true;

    newShaderFragmentSource = fragmentSource;
    newShaderVertexSource = "";
    newShaderIsContinuous = true;
    msg.push_back(MSG_NEW_SHADER);
    if (loopRunning)
        while (msg.back() == MSG_NEW_SHADER);
    return compileError;
}

// The main rendering loop
void Renderer::loop() {
    if (DEBUG)
        LOGD(LOG_TAG_RENDERER, "ENTERING LOOP");

    unsigned int frames = 0;
    frameRate = 0.0;
    auto startFps = std::chrono::steady_clock::now();
    auto endFps = std::chrono::steady_clock::now();
    auto startDraw = std::chrono::steady_clock::now();
    auto endDraw = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedFps = std::chrono::duration<double>(0);
    std::chrono::duration<double> elapsedDraw = std::chrono::duration<double>(0);
    double ms = 100.0;
    // MAX_FPS==0.1 draw 1 frame at max every 10 ms (max 100 FPS)
    double MAX_FPS = 1.0 / 100.0;
    loopRunning = true;
#if defined _IS_ANDROID_ || defined _IS_WIN_
    msg.push_back(MSG_INIT_OPENGL);
#endif

    Sampler2D *sampler;
    RenderThreadMessage _msg;

    while (loopRunning) {
        mutex.lock();

        if (msg.size() == 0) _msg = MSG_NONE;
        else { _msg = msg.back(); msg.pop_back(); }

        switch (_msg) {
            case MSG_INIT_OPENGL:
                #if defined _IS_ANDROID_ || defined _IS_WIN_
                    // On Android the GL context is created in initOpenGL()
                    // and must be created in the loop thread.
                    // On linux it is created by the Flutter engine
                    // initOpenGL() must be called and if a shader
                    // is already been set, initialize it!
                    if (!initOpenGL()) {
                        LOGD(LOG_TAG_RENDERER, "ERROR: Main Loop error initializing OpenGL!");
                        msg.push_back(MSG_STOP_RENDERER);
                    }
                    if (shader != nullptr)
                        msg.push_back(MSG_NEW_SHADER);
                #endif
                break;

            case MSG_NEW_SHADER:
                // Eventually stop the camera
                stopCamera();
                
                if (shader.get() != nullptr)
                    shader.reset();
                shader = std::make_unique<Shader>(self);
                shader->setShadersText(newShaderVertexSource, newShaderFragmentSource);
                shader->setShadersSize(self->width, self->height);
                shader->setIsContinuous(newShaderIsContinuous);

                if (isShaderToy)
                    compileError = shader->initShaderToy();
                else
                    compileError = shader->initShader();
                break;

            case MSG_START_CAMERA_ON_UNIFORM:
                #ifdef _IS_LINUX_
                    gdk_gl_context_make_current(self->context);
                #elif _IS_WIN_
                    wglMakeCurrent(self->hdc, self->hrc);
                #endif

                sampler = shader->getUniforms().getSampler2D(uniformToSetCamera);
                if (sampler != nullptr && camera != nullptr) 
                    camera->start(sampler);

                #ifdef _IS_LINUX_
                    gdk_gl_context_clear_current();
                #endif
                break;

            case MSG_NEW_TEXTURE:
                #ifdef _IS_LINUX_
                    gdk_gl_context_make_current(self->context);
                #elif _IS_WIN_
                    wglMakeCurrent(self->hdc, self->hrc);
                #endif
                    shader->getUniforms().setAllSampler2D();
                #ifdef _IS_LINUX_
                    gdk_gl_context_clear_current();
                #endif
            break;

            case MSG_SET_TEXTURE:
                #ifdef _IS_LINUX_
                        gdk_gl_context_make_current(self->context);
                #elif _IS_WIN_
                        wglMakeCurrent(self->hdc, self->hrc);
                #endif

                shader->getUniforms().setSampler2D("iChannel0", shader->getUniforms().programObject, sampler2DToSet);
                shader->getUniforms().setAllSampler2D();
                
                #ifdef _IS_LINUX_
                        gdk_gl_context_clear_current();
                #endif
            break;

            case MSG_DELETE_TEXTURE:
                #ifdef _IS_LINUX_
                        gdk_gl_context_make_current(self->context);
                #elif _IS_WIN_
                        wglMakeCurrent(self->hdc, self->hrc);
                #endif
                        glDeleteTextures(1, &textureIdToDelete);
                #ifdef _IS_LINUX_
                        gdk_gl_context_clear_current();
                #endif
            break;

            case MSG_STOP_RENDERER:
                loopRunning = false;
                #if defined _IS_ANDROID_ || defined _IS_WIN_
                    destroyGL();
                #endif
                break;

            default:
                if (shader == nullptr || !shader->isContinuous())
                    break;
                // TODO check this frameRate because visually it seems not correct!
                // TODO Maybe there is some asynchronous function that doesn't let counting right?
                elapsedFps = endFps - startFps;
                elapsedDraw = endDraw - startDraw;

                if (elapsedDraw.count() >= MAX_FPS) {
                    frames++;
                    // printf("%d\n", frames); fflush(stdout);
                    shader->drawFrame();
                    startDraw = std::chrono::steady_clock::now();
                    // this doesn't seem to correct FPS (OpenGL has some asynchronous operations)
                    glFlush();
                    glFinish();
                }
                endDraw = std::chrono::steady_clock::now();

                // update frameRate every second
                if (elapsedFps.count() >= 1.0) {
                    frameRate = (double) frames * 0.5 + frameRate * 0.5; // more stable
                    frames = 0;
                    startFps = std::chrono::steady_clock::now();
                }
                endFps = std::chrono::steady_clock::now();
                break;
        }

        mutex.unlock();
    }
    loopRunning = false;
}