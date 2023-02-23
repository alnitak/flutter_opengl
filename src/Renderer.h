#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "Shader.h"

#ifdef _IS_ANDROID_

    #include <android/native_window.h>
    #include <android/native_window_jni.h>
    #include <android/surface_texture.h>
    #include <android/surface_texture_jni.h>
    #include "opencv_capture.h"

#elif _IS_LINUX_
    #include "../linux/include/fl_my_texture_gl.h"
    #include "opencv_capture.h"
#elif _IS_WIN_
    #include "../windows/flutter_opengl_plugin.h"
    #include "opencv_capture.h"
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

    inline std::string getCompileError() { return compileError; };

    inline Shader *getShader() { return shader.get(); };

    OpenCVCapture *getOpenCVCapture();

    inline bool isLooping() { return loopRunning; };

    inline double getFrameRate() { return frameRate; };

    inline void setStartCameraOnUniformMsg(const std::string &name) {
        uniformToSetCapture = name;
        msg.push_back(MSG_START_CAPTURE_ON_UNIFORM);
    };

    inline void setNewTextureMsg() { msg.push_back(MSG_NEW_TEXTURE); };

    // set new data for a texture of the same size
    inline void setTextureMsg(const Sampler2D &sampler) { 
        sampler2DToSet = sampler; 
        msg.push_back(MSG_SET_TEXTURE);
    };

    inline void deleteTextureMsg(unsigned int textureId) { 
        textureIdToDelete = textureId; 
        msg.push_back(MSG_DELETE_TEXTURE);
    };

    bool openCapture(std::string uniformName,
                    std::string completeFilePath,
                    int *width, int *height);

    bool stopCapture();

private:
    OpenglPluginContext *self;
    std::mutex mutex;
    double frameRate;

    OpenCVCapture *capture;

    std::string compileError;
    std::unique_ptr<Shader> shader;
    bool newShaderIsContinuous;
    std::string newShaderFragmentSource;
    std::string newShaderVertexSource;

    bool isShaderToy;
    bool loopRunning;
    Sampler2D sampler2DToSet;
    std::string uniformToSetCapture;
    unsigned int textureIdToDelete;

    enum RenderThreadMessage : int {
        MSG_NONE = 0,
        MSG_DATA_RECEIVED,   // when new data is ready, the draw function is called which must then free its memory
        MSG_INIT_OPENGL, // only in Android, this must be called in the loop thread(?)
        MSG_STOP_RENDERER,
        MSG_NEW_SHADER,
        MSG_NEW_TEXTURE,
        MSG_DELETE_TEXTURE,
        MSG_SET_TEXTURE,    // set new data for a texture of the same size
        MSG_START_CAPTURE_ON_UNIFORM
    };
    std::vector<RenderThreadMessage> msg;

};

#endif //RENDERER_H