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

package com.bavagnoli.opengl;

import android.graphics.SurfaceTexture;
import android.view.Surface;

public class JNIUtils {
    private static final String TAG = "JNIUtils";

    static {
        System.loadLibrary("native-lib");
    }

    public static native void nativeSetSurface(Surface surface,
                                               int func,
                                               String debugName,
                                               int width, int height,
                                               float scaleX, float scaleY,
                                               int clearR, int clearG, int clearB, int clearA);
    public static native void nativeOnStop();
    public static native int nativeSurfaceIsContextValid();
    public static native boolean nativeSurfaceIsLoopingRunning();
    public static native boolean nativeSurfaceIsThreadRunning();
    public static native void nativeSurfaceSetClearColor(int clearR, int clearG, int clearB, int clearA);

}
