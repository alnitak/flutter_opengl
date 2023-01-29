#ifndef FFI_H
#define FFI_H

#include "common.h"
#include "uniformQueue.h"
#include "Renderer.h"

void deleteRenderer();

void createRenderer(OpenglPluginContext *textureStruct);

Renderer *getRenderer();

extern "C" FFI_PLUGIN_EXPORT bool rendererStatus();
extern "C" FFI_PLUGIN_EXPORT void getTextureSize(int32_t *width, int32_t *height);
extern "C" FFI_PLUGIN_EXPORT void startThread();
extern "C" FFI_PLUGIN_EXPORT void stopThread();
extern "C" FFI_PLUGIN_EXPORT const char *setShader(
        bool isContinuous,
        const char *vertexShader,
        const char *fragmentShader);
extern "C" FFI_PLUGIN_EXPORT const char *setShaderToy(
        const char *fragmentShader);
extern "C" FFI_PLUGIN_EXPORT const char *getVertexShader();
extern "C" FFI_PLUGIN_EXPORT const char *getFragmentShader();
extern "C" FFI_PLUGIN_EXPORT void addShaderToyUniforms();
extern "C" FFI_PLUGIN_EXPORT void setMousePosition(
        double posX,
        double posY,
        double posZ,
        double posW,
        double textureWidgetWidth, 
        double textureWidgetHeight);
extern "C" FFI_PLUGIN_EXPORT double getFPS();
extern "C" FFI_PLUGIN_EXPORT bool addUniform(
        const char *name,
        UniformType type,
        void *val);
extern "C" FFI_PLUGIN_EXPORT bool setUniform(
        const char *name,
        void *val);
extern "C" FFI_PLUGIN_EXPORT bool addSampler2DUniform(
        const char *name, 
        int width, 
        int height, 
        void *val);

#endif // FFI_H