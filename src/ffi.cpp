#include "common.h"
#include "ffi.h"
#include "Renderer.h"
#include "uniformQueue.h"
#include "Sampler2D.h"


Renderer *renderer = nullptr;

#define LOG_TAG_FFI "NATIVE FFI"
#define DEBUG_TAG_FFI true

////////////////////////////////
/// renderer
////////////////////////////////
void deleteRenderer() {
    if (renderer != nullptr) {
        if (renderer->isLooping()) {
            // eventually stop the capture
            #ifdef WITH_OPENCV
                if (renderer->getOpenCVCapture() != nullptr) renderer->stopCapture();
            #endif

            while (bool b = renderer->isLooping()) renderer->stop();
        }
        delete renderer;
        renderer = nullptr;
    }
}

void createRenderer(OpenglPluginContext *textureStruct) {
    deleteRenderer();
    renderer = new Renderer(textureStruct);
}

Renderer *getRenderer() { return renderer; }

extern "C" FFI_PLUGIN_EXPORT bool rendererStatus() {
    if (renderer == nullptr) return false;
    return true;
}

extern "C" FFI_PLUGIN_EXPORT void getTextureSize(int32_t *width, int32_t *height) {
    if (renderer == nullptr || renderer->getShader() == nullptr) {
        *width = -1;
        *height = -1;
        return;
    }
    *width = renderer->getShader()->getWidth();
    *height = renderer->getShader()->getHeight();
}


///////////////////////////////////
// Start OpenGL thread
extern "C" FFI_PLUGIN_EXPORT void
startThread() {
    if (renderer == nullptr) {
        LOGD(LOG_TAG_FFI, "startThread: Texture not yet created!");
        return;
    }
    std::thread loopThread = std::thread([&]() {
        renderer->loop();
    });
    loopThread.detach();
}

///////////////////////////////////
// Stop OpenGL thread
extern "C" FFI_PLUGIN_EXPORT void
stopThread() {
    if (renderer == nullptr) {
        LOGD(LOG_TAG_FFI, "stopThread: Renderer not yet created!");
        return;
    }
    renderer->stop();
    while (renderer->isLooping());
    delete renderer;
    renderer = nullptr;
}

///////////////////////////////////
// Set new shader
std::string compileError;
extern "C" FFI_PLUGIN_EXPORT const char *
setShader(bool isContinuous,
          const char *vertexShader, const char *fragmentShader) {
    if (renderer == nullptr) {
        LOGD(LOG_TAG_FFI, "setShader: Renderer not yet created!");
        return "";
    }
    compileError = renderer->setShader(
            isContinuous,
            vertexShader,
            fragmentShader);
    const char *ret = compileError.c_str();
    return ret;
}

///////////////////////////////////
// Set new ShaderToy shader
extern "C" FFI_PLUGIN_EXPORT const char *
setShaderToy(const char *fragmentShader) {
    if (renderer == nullptr) {
        LOGD(LOG_TAG_FFI, "setShaderToy: Renderer not yet created!");
        return "";
    }

    compileError = renderer->setShaderToy(fragmentShader);
    const char *ret = compileError.c_str();
    return ret;
}

///////////////////////////////////
// Get shader sources
extern "C" FFI_PLUGIN_EXPORT const char *
getVertexShader() {
    if (renderer == nullptr) {
        LOGD(LOG_TAG_FFI, "getVertexShader: Renderer not yet created!");
        return "";
    }
    if (renderer->getShader() == nullptr) {
        LOGD(LOG_TAG_FFI, "getVertexShader: Shader not set yet!");
        return "";
    }
    return renderer->getShader()->vertexSource.c_str();
}

extern "C" FFI_PLUGIN_EXPORT const char *
getFragmentShader() {
    if (renderer == nullptr) {
        LOGD(LOG_TAG_FFI, "getFragmentShader: Renderer not yet created!");
        return "";
    }
    if (renderer->getShader() == nullptr) {
        LOGD(LOG_TAG_FFI, "getFragmentShader: Shader not set yet!");
        return "";
    }
    return renderer->getShader()->fragmentSource.c_str();
}
///////////////////////////////////
// Set mouse position
extern "C" FFI_PLUGIN_EXPORT void
addShaderToyUniforms() {
    if (renderer == nullptr) {
        LOGD(LOG_TAG_FFI, "addShaderToyUniforms: Renderer not yet created!");
        return;
    }
    if (renderer->getShader() == nullptr) {
        LOGD(LOG_TAG_FFI, "addShaderToyUniforms: Shader not set yet!");
        return;
    }
    renderer->getShader()->addShaderToyUniforms();
}

///////////////////////////////////
// Set mouse position
// Shows how to use the mouse input (only left button supported):
//
//      mouse.xy  = mouse position during last button down
//  abs(mouse.zw) = mouse position during last button click
// sign(mouze.z)  = button is down
// sign(mouze.w)  = button is clicked
// https://www.shadertoy.com/view/llySRh
// https://www.shadertoy.com/view/Mss3zH
extern "C" FFI_PLUGIN_EXPORT void
setMousePosition(
    double posX, double posY, double posZ, double posW,
    double textureWidgetWidth, double textureWidgetHeight) {
    if (renderer == nullptr) {
        LOGD(LOG_TAG_FFI, "setMousePosition: Renderer not yet created!");
        return;
    }
    if (renderer->getShader() == nullptr) {
        LOGD(LOG_TAG_FFI, "setMousePosition: Shader not yet binded!");
        return;
    }
    // Normalize values from the Texture() widget to the texture size 
    double textureWidth = renderer->getShader()->getWidth();
    double textureHeigth = renderer->getShader()->getHeight();
    double arH = textureWidth / textureWidgetWidth;
    double arV = textureHeigth / textureWidgetHeight;
    posX *= arH;
    posY *= arV;
    posZ *= arH;
    posW *= arV;
    // flip vertically Y coord
    posY = textureHeigth - posY;
    posW = -textureHeigth - posW;
    // LOGD(LOG_TAG_FFI, "setMousePosition: %f %f %f %f", posX, posY, posZ, posW);
    auto mouse = glm::vec4(
        posX, 
        posY,
        posZ, 
        posW);
    renderer->getShader()->getUniforms().setUniformValue(
            std::string("iMouse"),
            (void *) (&mouse));
}

///////////////////////////////////
// Get FPS
extern "C" FFI_PLUGIN_EXPORT double
getFPS() {
    if (renderer == nullptr) {
        LOGD(LOG_TAG_FFI, "getFPS: Renderer not yet created!");
        return -1.0;
    }
    if (!renderer->isLooping()) {
        LOGD(LOG_TAG_FFI, "getFPS: Renderer not running!");
        return -1.0;
    }
    return renderer->getFrameRate();
}

///////////////////////////////////
// Add uniform
extern "C" FFI_PLUGIN_EXPORT bool
addUniform(
        const char *name,
        UniformType type,
        void *val) {
    if (renderer == nullptr) {
        LOGD(LOG_TAG_FFI, "addUniform: Renderer not yet created!");
        return false;
    }
    if (renderer->getShader() == nullptr) {
        LOGD(LOG_TAG_FFI, "addUniform: shader not yet binded!");
        return false;
    }
    renderer->getShader()->getUniforms().addUniform(name, type, val);
    return true;
}

///////////////////////////////////
// Remove uniform
extern "C" FFI_PLUGIN_EXPORT bool
removeUniform(const char *name) {
    if (renderer == nullptr) {
        LOGD(LOG_TAG_FFI, "addUniform: Renderer not yet created!");
        return false;
    }
    if (renderer->getShader() == nullptr) {
        LOGD(LOG_TAG_FFI, "addUniform: shader not yet binded!");
        return false;
    }
    return renderer->getShader()->getUniforms().removeUniform(name);
}

///////////////////////////////////
// Set uniform
extern "C" FFI_PLUGIN_EXPORT bool
setUniform(
        const char *name,
        void *val) {
    if (renderer == nullptr) {
        LOGD(LOG_TAG_FFI, "addUniform: Renderer not yet created!");
        return false;
    }
    if (renderer->getShader() == nullptr) {
        LOGD(LOG_TAG_FFI, "addUniform: shader not yet binded!");
        return false;
    }
    return renderer->getShader()->getUniforms().setUniformValue(name, val);
}

///////////////////////////////////
// Add Sampler2D uniform
//* val should be a list of RGBA32 values
//* Use setUniform() to set a new value with a texture with the same size
extern "C" FFI_PLUGIN_EXPORT bool
addSampler2DUniform(const char *name, int width, int height, void *val)
{
    if (renderer == nullptr) {
        LOGD(LOG_TAG_FFI, "addUniform: Renderer not yet created!");
        return false;
    }
    if (renderer->getShader() == nullptr) {
        LOGD(LOG_TAG_FFI, "addUniform: shader not yet binded!");
        return false;
    }
    bool ret = false;
    Sampler2D sampler = Sampler2D();
    sampler.add_RGBA32(width, height, (unsigned char*)val);
    ret = renderer->getShader()->getUniforms()
        .addUniform(name, UNIFORM_SAMPLER2D, (void*)&sampler);

    if (ret && renderer->isLooping())
        renderer->setNewTextureMsg();
    return ret;
}

///////////////////////////////////
// Replace Sampler2D uniform
//* val should be a list of RGBA32 values
//* Used when replacing textures of different sizes
extern "C" FFI_PLUGIN_EXPORT bool
replaceSampler2DUniform(const char *name, int width, int height, void *val)
{
    if (renderer == nullptr) {
        LOGD(LOG_TAG_FFI, "replaceSampler2DUniform: Renderer not yet created!");
        return false;
    }
    if (renderer->getShader() == nullptr) {
        LOGD(LOG_TAG_FFI, "replaceSampler2DUniform: shader not yet binded!");
        return false;
    }
    bool replaced = renderer->getShader()->getUniforms()
        .replaceSampler2D(name, width, height, (unsigned char *)val);

    if (replaced && renderer->isLooping())
        renderer->setNewTextureMsg();
    return replaced;
}



///////////////////////////////////
// Start capture
extern "C" FFI_PLUGIN_EXPORT bool
startCaptureOnSampler2D(const char *name, const char *completeFilePath) {
    #ifndef WITH_OPENCV
        LOGD(LOG_TAG_FFI, "Error startCaptureOnSampler2D(): OPENCV is not yet supported on this platform!");
        return false;
    #else
        if (renderer == nullptr) {
            LOGD(LOG_TAG_FFI, "startCaptureOnSampler2D: Renderer not yet created!");
            return false;
        }

        int width;
        int height;
        if (renderer->openCapture(name, completeFilePath, &width, &height)) {
            renderer->setStartCameraOnUniformMsg(name);
            // Force a resample of sampler2D texture
    //        char dummy[width * height * 4];
            auto * dummy = (unsigned char *) malloc(width * height * 4);
            replaceSampler2DUniform(name, width, height, (void *) dummy);
            free(dummy);
            return true;
        }

        return false;
    #endif
}

///////////////////////////////////
// Start capture
extern "C" FFI_PLUGIN_EXPORT bool
stopCapture()
{
    #ifndef WITH_OPENCV
        LOGD(LOG_TAG_FFI, "Error stopCapture(): OPENCV is not yet supported on this platform!");
        return false;
    #else
        if (renderer == nullptr) {
            LOGD(LOG_TAG_FFI, "stopCapture: Renderer not yet created!");
            return false;
        }
        return renderer->stopCapture();
    #endif
}
