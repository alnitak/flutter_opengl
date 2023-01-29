#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "Shader.h"

#ifdef _IS_ANDROID_

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/surface_texture.h>
#include <android/surface_texture_jni.h>

#elif _IS_LINUX_
#include "../linux/include/fl_my_texture_gl.h"
#elif _IS_WIN_
#include "../windows/flutter_opengl_plugin.h"
#endif


#include <thread>
#include <mutex>

class Renderer {

public:
    Renderer(
            OpenglPluginContext *textureStruct);

    ~Renderer();

#ifdef _IS_ANDROID_

    bool setNativeAndroidWindow(jobject surface);

    bool initOpenGL();

    void destroyGL();

#endif
#ifdef _IS_WIN_
    GLint setPixelFormat();
    bool initOpenGL();
    void destroyGL();
#endif

    void stop();

    void loop();

    std::string setShader(bool isContinuous, const char *vertexSource, const char *fragmentSource);

    std::string setShaderToy(const char *fragmentSource);

    std::string getCompileError() { return compileError; };

    Shader *getShader() { return shader.get(); };

    bool isLooping() { return loopRunning; };

    double getFrameRate() { return frameRate; };

    void setNewTextureMsg() { msg = MSG_NEW_TEXTURE; };

private:
    OpenglPluginContext *self;
    std::mutex mutex;
    double frameRate;

    std::string compileError;
    std::unique_ptr<Shader> shader;
    bool newShaderIsContinuous;
    std::string newShaderFragmentSource;
    std::string newShaderVertexSource;

    bool isShaderToy;
    bool loopRunning;
    bool isDrawing;

    enum RenderThreadMessage {
        MSG_NONE = 0,
        MSG_DATA_RECEIVED,   // when new data is ready, the draw function is called which must then free its memory
        MSG_INIT_OPENGL, // only in Android, this must be called in the loop thread(?)
        MSG_STOP_RENDERER,
        MSG_NEW_SHADER,
        MSG_NEW_TEXTURE,
    };
    enum RenderThreadMessage msg;

};

#endif //RENDERER_H