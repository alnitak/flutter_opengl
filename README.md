# flutter_opengl

Flutter OpenGL ES plugin.

## Getting Started

| Android | Windows | Linux | iOS | MacOS | Web|
| ---- | ---- | ---- | ---- | ---- | ---- |
| ✅  | ✅ | ✅ | x | x | x|

![gif](https://github.com/alnitak/flutter_opengl/blob/master/images/flutter_opengl.gif?raw=true "Flutter OpenGL Demo")

The main workflow of the plugin is:

- ask to native code with a MethodChannel for a texture ID
- use the texture ID in a Texture() widget
- set a vertex and a fragment sources
- start the renderer
- change shader sources on the fly

All functionalities, but the first call to the first method channel, use FFI calls.

The starting idea developing this plugin, was not just to use GLSL, but also take advantage of the wonderful [ShaderToy](https://www.shadertoy.com/) web site.

For now it's possible to copy/paste shaders from ShaderToy, but only those which have only one layer (ie no iChannelN, iSound etc).

On a real device, many of them could be very slow because they are hungry of power and some others needs ES 3 and for now it cannot be compiled on Android (ie shaders 13, 14 and 15 in the example).

***iResolution***, ***iTime*** and ***iMouse*** are supported, other uniforms can be added at run-time.

- ***iResolution*** is a vec3 uniform which represents the texture size
- ***iTime*** is a float which represents the time since the shader was created
- ***iMouse*** is a vec4 which the x and y values represent the coordinates where the mouse or the touch is grabbed hover the *Texture()* widget




# Setup

## Linux
Be sure you have installed **glew** and **glm** packages.


## Windows
Go into the windows folder from the project root.
-  clone **Native_SDK**:

```git clone https://github.com/powervr-graphics/Native_SDK.git```

you can safely delete all but the *lib* and *include* directories from the cloned repo

- clone **glm**

```git clone https://github.com/g-truc/glm.git```

- download **glew** *Binaries for Windows 32-bit and 64-bit* from here:

[https://glew.sourceforge.net](https://glew.sourceforge.net/) (sources at https://github.com/nigels-com/glew)

extract the zip and rename its main directory to "glew"

## Android
Should be ok.

# TODO
- the c/c++ code is not "state of the art" written! PRs are welcomed :smile:
- iOS, Mac and Web support
- add textures (ie iChannel0 used on ShaderToy). A texture of 2^N x 2 could be used for example for FFT audio samples on iSound ShaderToy uniform.
- more then one parallel shaders
- ES 3 on Android (now supports 2)
- displayed FPS seems not to be correct
- use of this plugin not just for shader but also for 3D models
