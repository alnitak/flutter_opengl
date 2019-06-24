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


#ifndef ANDROID_RENDERERCONTROLLER_H
#define ANDROID_RENDERERCONTROLLER_H

#include "ndk/common.hpp"
#include "renderer.h"

#include "Shader.h"
#include "shaders/Cube.h"


class RendererController {

public:
    RendererController();
    ~RendererController();

    void nativeSetSurface(JNIEnv* jenv,
                          jobject surface,
                          int func,
                          const char *name,
                          int width, int height,
                          float scaleX, float scaleY,
                          int clearR, int clearG, int clearB, int clearA);

    void nativeOnStop();

    void setNativeWindowClearColor(const unsigned char r,
                                   const unsigned char g,
                                   const unsigned char b,
                                   const unsigned char a);

    Renderer *renderer;

private:
    pthread_t _renderer_thread;
    pthread_mutex_t _mutex_controller;

    Shader *shader;
};



#endif //ANDROID_RENDERERCONTROLLER_H
