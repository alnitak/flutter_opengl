# OpenGL Flutter plugin

OpenGL Android NDK plugin.

[video](https://www.youtube.com/watch?v=GKEdwZsAb0s)

## Getting Started

This plugin is only for Android. Is greatly appreciated any help!  
  
The example provides 4 drawing shaders. They are coded in `shaders.cpp`.
Each of them need an `initShader(void *args)` and a `drawFrame(void *args)` functions which are passed to the
renderer.cpp engine at creation time. `RendererController::nativeSetSurface()` has the task to choose which paired
function have to be passed to the renderer engine.  

The 3 example shaders are taken from https://www.shadertoy.com/
https://www.shadertoy.com/view/3l23Rh by nimitz https://www.shadertoy.com/user/nimitz
https://www.shadertoy.com/view/XsXXDn by Danguafer https://www.shadertoy.com/user/Danguafer 
https://www.shadertoy.com/view/ttlGDf by alro https://www.shadertoy.com/user/alro

(more to come)


##### ISSUEs:
shader #2 doesn't work on Huawei MediaPad M5: graphics adapter ARM Mali-G71 MP8
___
Currently NDK builds for x86_64, x86, arm64-v8a and armeabi-v7a and libnative-lib.so is available on all ABI lib subfolders
but libflutter.so is missing in armeabi-v7a.
In example or plugin build.gradle or both, forcing gradle to build only for armeabi-v7a with
```
ndk {
    abiFilters 'armeabi-v7a'
}
```
doesn't work and libflutter.so is missing in the lib folder, but works with command line:
```
flutter run --debug --target-platform android-arm
```

(more to come)
 
##### TODO: 
`Renderer::getWindowWidth()` and `Renderer::getWindowHeight()` doesn't work correctly:  
they are used in `Renderer::initializeGL()` to set window buffer size with `ANativeWindow_setBuffersGeometry()`,
without it the window `surface` is not working and nothing is displayed.
___
Add source comments.
___
Implement a way to pass an OpenGL texture from Flutter to NDK.
___
Implement iOS plugin.
___
Update code to at least c++14

(more to come)
