# OpenGL Flutter plugin

OpenGL Android NDK plugin.


[YT video](https://www.youtube.com/watch?v=GKEdwZsAb0s)

![Image](https://github.com/alnitak/flutter_opengl/blob/master/flutter_01.png?raw=true)

## Getting Started

This plugin is only for Android. Is greatly appreciated any help!  
This is an early stage of a plugin for Flutter to let the developers to use an OpenGL widget.
This is not a binding to let the devs to write OpenGL code within Flutter,
but lets to write shader and drawing frame functions in C/C++ code.
The plugin is only available for Android OS, looking forward for someone to make it cross-platform!
  
The example provides 4 drawing shaders classes. They are subclass of `Shader.cpp`.
Each of them need an `initShader(void *args)` and a `drawFrame(void *args)` functions which are used by the
renderer.cpp engine at creation time. `RendererController::nativeSetSurface()` has the task to crate
the renderer engine.  



The 3 example shaders are taken from https://www.shadertoy.com/
https://www.shadertoy.com/view/3l23Rh by nimitz https://www.shadertoy.com/user/nimitz
https://www.shadertoy.com/view/llj3Dz by Darthmarshie https://www.shadertoy.com/user/Darthmarshie
https://www.shadertoy.com/view/ttlGDf by alro https://www.shadertoy.com/user/alro

(more to come)


##### ISSUEs:
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
Implement iOS layer plugin.

(more to come)