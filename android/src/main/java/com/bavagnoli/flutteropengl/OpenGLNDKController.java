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

package com.bavagnoli.flutteropengl;

import android.graphics.SurfaceTexture;
import android.view.Surface;

public class OpenGLNDKController {
    static String TAG = OpenGLNDKController.class.getSimpleName();

    protected final SurfaceTexture texture;


    /**
     *
     * @param texture texture ID from Texture() widget
     * @param drawingFunction to choose which shader and frame drawing function to use
     * @param width   force engine to know surface texture widget (see readme.MD)
     * @param height
     */
    public OpenGLNDKController(SurfaceTexture texture, int drawingFunction, int width, int height) {
        this.texture = texture;

        JNIUtils.nativeSetSurface(
                new Surface(texture),
                drawingFunction,
                "test1", // MAX 50 chars
                width, height,
                1.0F, 1.0F,
                100,0, 0, 255
        );
    }

    public void stop() {
        JNIUtils.nativeOnStop();
    }


}
