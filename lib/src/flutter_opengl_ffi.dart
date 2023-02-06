import 'dart:ffi' as ffi;
import 'dart:typed_data';

import 'package:ffi/ffi.dart';
import 'package:flutter/material.dart';

enum PointerEventType {
  onPointerDown,
  onPointerMove,
  onPointerUp,
}

enum UniformType {
  uniformBool,
  uniformInt,
  uniformFloat,
  uniformVec2,
  uniformVec3,
  uniformVec4,
  uniformMat2,
  uniformMat3,
  uniformMat4,
  uniformSampler2D,
}

/// FFI bindings to Flutter_OpenGL
class FlutterOpenGLFfi {
  /// Holds the symbol lookup function.
  final ffi.Pointer<T> Function<T extends ffi.NativeType>(String symbolName)
      _lookup;

  /// The symbols are looked up in [dynamicLibrary].
  FlutterOpenGLFfi(ffi.DynamicLibrary dynamicLibrary)
      : _lookup = dynamicLibrary.lookup;

  /// The symbols are looked up with [lookup].
  FlutterOpenGLFfi.fromLookup(
      ffi.Pointer<T> Function<T extends ffi.NativeType>(String symbolName)
          lookup)
      : _lookup = lookup;

  /// ***********************************************
  /// **** GET RENDERER STATUS
  /// Returns true if the texture has been created and a shader can be choosen
  bool rendererStatus() {
    return _rendererStatus() == 0 ? false : true;
  }

  late final _rendererStatusPtr =
      _lookup<ffi.NativeFunction<ffi.Int Function()>>('rendererStatus');
  late final _rendererStatus = _rendererStatusPtr.asFunction<int Function()>();

  /// ***********************************************
  /// **** GET TEXTURE SIZE
  /// Get the size of the shader already set.
  /// If not set it return Size(-1, -1)
  Size getTextureSize() {
    ffi.Pointer<ffi.Int32> w = calloc(ffi.sizeOf<ffi.Int32>());
    ffi.Pointer<ffi.Int32> h = calloc(ffi.sizeOf<ffi.Int32>());

    _textureSize(w, h);
    Size size = Size(w.value.toDouble(), h.value.toDouble());

    calloc.free(w);
    calloc.free(h);
    return size;
  }

  late final _textureSizePtr = _lookup<
      ffi.NativeFunction<
          ffi.Int Function(ffi.Pointer<ffi.Int32>,
              ffi.Pointer<ffi.Int32>)>>('getTextureSize');
  late final _textureSize = _textureSizePtr.asFunction<
      int Function(ffi.Pointer<ffi.Int32>, ffi.Pointer<ffi.Int32>)>();

  /// ***********************************************
  /// **** START THREAD
  /// Starts the drawing thread loop. It does nothing it's already running
  void startThread() {
    return _startThread();
  }

  late final _startThreadPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function()>>('startThread');
  late final _startThread = _startThreadPtr.asFunction<void Function()>();

  /// ***********************************************
  /// **** STOP THREAD
  /// Delete shader, delete texture and stops the drawing thread loop
  void stopThread() {
    return _stopThread();
  }

  late final _stopThreadPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function()>>('stopThread');
  late final _stopThread = _stopThreadPtr.asFunction<void Function()>();

  /// ***********************************************
  /// **** SET SHADER
  /// Set the shader to be used in the current texture.
  String setShader(
    bool isContinuous,
    String vertexShader,
    String fragmentShader,
  ) {
    ffi.Pointer<ffi.Char> err = _setShader(
      isContinuous ? 1 : 0,
      vertexShader.toNativeUtf8().cast<ffi.Char>(),
      fragmentShader.toNativeUtf8().cast<ffi.Char>(),
    );
    String ret = err.cast<Utf8>().toDartString();
    return ret;
  }

  late final _setShaderPtr = _lookup<
      ffi.NativeFunction<
          ffi.Pointer<ffi.Char> Function(ffi.Int, ffi.Pointer<ffi.Char>,
              ffi.Pointer<ffi.Char>)>>('setShader');
  late final _setShader = _setShaderPtr.asFunction<
      ffi.Pointer<ffi.Char> Function(
          int, ffi.Pointer<ffi.Char>, ffi.Pointer<ffi.Char>)>();

  /// ***********************************************
  /// **** SET SHADERTOY SHADER
  /// Set the shader to be used in the current texture.
  /// These are only fragment shaders taken from ShaderToy.com
  /// Many of the shaders can be copy/pasted, but they must have
  /// only the "image" layer (no iChannel, no buffer etc).
  /// Also many of them are heavy for mobile devices (few FPS).
  /// The uniforms actually available and aumatically registered are:
  /// float iTime
  /// vec4 iMouse
  /// vec3 iResolution
  String setShaderToy(String fragmentShader) {
    return _setShaderToy(
      fragmentShader.toNativeUtf8().cast<ffi.Char>(),
    ).cast<Utf8>().toDartString();
  }

  late final _setShaderToyPtr = _lookup<
      ffi.NativeFunction<
          ffi.Pointer<ffi.Char> Function(
              ffi.Pointer<ffi.Char>)>>('setShaderToy');
  late final _setShaderToy = _setShaderToyPtr
      .asFunction<ffi.Pointer<ffi.Char> Function(ffi.Pointer<ffi.Char>)>();

  /// ***********************************************
  /// **** GET VERTEX SHADER
  /// Get current vertex shader text
  String getVertexShader() {
    ffi.Pointer<ffi.Char> vs = _getVertexShader();
    return vs.cast<Utf8>().toDartString();
  }

  late final _getVertexShaderPtr =
      _lookup<ffi.NativeFunction<ffi.Pointer<ffi.Char> Function()>>(
          'getVertexShader');
  late final _getVertexShader =
      _getVertexShaderPtr.asFunction<ffi.Pointer<ffi.Char> Function()>();

  /// ***********************************************
  /// **** GET FRAGMENT SHADER
  /// Get current fragment shader text
  String getFragmentShader() {
    ffi.Pointer<ffi.Char> fs = _getFragmentShader();
    return fs.cast<Utf8>().toDartString();
  }

  late final _getFragmentShaderPtr =
      _lookup<ffi.NativeFunction<ffi.Pointer<ffi.Char> Function()>>(
          'getFragmentShader');
  late final _getFragmentShader =
      _getFragmentShaderPtr.asFunction<ffi.Pointer<ffi.Char> Function()>();

  /// ***********************************************
  /// **** ADD SHADERTORY UNIFORMS
  /// this will add
  /// vec4 iMouse
  /// vec3 iResolution
  /// float iTime
  /// These are automatically sett when using [setShaderToy]
  void addShaderToyUniforms() {
    return _addShaderToyUniforms();
  }

  late final _addShaderToyUniformsPtr =
      _lookup<ffi.NativeFunction<ffi.Void Function()>>('addShaderToyUniforms');
  late final _addShaderToyUniforms =
      _addShaderToyUniformsPtr.asFunction<void Function()>();

  /// ***********************************************
  /// **** SET MOUSE POS
  /// Set the iMouse uniform
  // Shows how to use the mouse input (only left button supported):
  //
  //      mouse.xy  = mouse position during last button down
  //  abs(mouse.zw) = mouse position during last button click
  // sign(mouze.z)  = button is down
  // sign(mouze.w)  = button is clicked
  // https://www.shadertoy.com/view/llySRh
  // https://www.shadertoy.com/view/Mss3zH
  void setMousePosition(
    Offset startingPos,
    Offset pos,
    PointerEventType eventType,
    Size twSize,
  ) {
    return _setMousePosition(
      pos.dx,
      pos.dy,
      eventType == PointerEventType.onPointerDown ||
              eventType == PointerEventType.onPointerMove
          ? startingPos.dx
          : -startingPos.dx,
      -startingPos.dy,
      twSize.width,
      twSize.height,
    );
  }

  late final _setMousePositionPtr = _lookup<
      ffi.NativeFunction<
          ffi.Void Function(ffi.Double, ffi.Double, ffi.Double, ffi.Double,
              ffi.Double, ffi.Double)>>('setMousePosition');
  late final _setMousePosition = _setMousePositionPtr.asFunction<
      void Function(double, double, double, double, double, double)>();

  /// ***********************************************
  /// **** GET FPS
  /// Get current FPS (the algorithm seems correct, but what we see, is not!
  /// See Renderer::loop() )
  double getFps() {
    return _getFps();
  }

  late final _getFpsPtr =
      _lookup<ffi.NativeFunction<ffi.Double Function()>>('getFPS');
  late final _getFps = _getFpsPtr.asFunction<double Function()>();

  /// ***********************************************
  /// **** SET CLEAR COLOR (not used yet)
  void setClearColor(
    int clearR,
    int clearG,
    int clearB,
    int clearA,
  ) {
    return _nativeSurfaceSetClearColor(
      clearR,
      clearG,
      clearB,
      clearA,
    );
  }

  late final _nativeSurfaceSetClearColorPtr = _lookup<
      ffi.NativeFunction<
          ffi.Void Function(ffi.Int32, ffi.Int32, ffi.Int32,
              ffi.Int32)>>('nativeSurfaceSetClearColor');
  late final _nativeSurfaceSetClearColor = _nativeSurfaceSetClearColorPtr
      .asFunction<void Function(int, int, int, int)>();

  /// ***********************************************
  /// ***********************************************
  /// ***********************************************
  /// ***********************************************
  /// **** ADD UNIFORMs
  /// * add BOOL
  bool addBoolUniform(String name, bool val) {
    ffi.Pointer<ffi.Bool> valT = calloc(ffi.sizeOf<ffi.Bool>());
    valT.value = val;

    int ret = _addUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      UniformType.uniformBool.index,
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  /// * add INT
  bool addIntUniform(String name, int val) {
    ffi.Pointer<ffi.Int32> valT = calloc(ffi.sizeOf<ffi.Int32>());
    valT.value = val;

    int ret = _addUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      UniformType.uniformInt.index,
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  /// * add FLOAT
  bool addFloatUniform(String name, double val) {
    ffi.Pointer<ffi.Float> valT = calloc(ffi.sizeOf<ffi.Float>());
    valT.value = val;

    int ret = _addUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      UniformType.uniformFloat.index,
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  /// * add VEC2
  bool addVec2Uniform(String name, List<double> val) {
    assert(
        val.length == 2,
        "Assert error: vec2 has 2 doubles."
        "You have passed a list with ${val.length}");
    ffi.Pointer<ffi.Float> valT = calloc(ffi.sizeOf<ffi.Float>() * 2);
    for (int i = 0; i < val.length; ++i) {
      valT[i] = val[i];
    }

    int ret = _addUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      UniformType.uniformVec2.index,
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  /// * add VEC3
  bool addVec3Uniform(String name, List<double> val) {
    assert(
        val.length == 3,
        "Assert error: vec3 has 3 doubles."
        "You have passed a list with ${val.length}");
    ffi.Pointer<ffi.Float> valT = calloc(ffi.sizeOf<ffi.Float>() * 3);
    for (int i = 0; i < val.length; ++i) {
      valT[i] = val[i];
    }

    int ret = _addUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      UniformType.uniformVec3.index,
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  /// * add VEC4
  bool addVec4Uniform(String name, List<double> val) {
    assert(
        val.length == 4,
        "Assert error: vec4 has 4 doubles."
        "You have passed a list with ${val.length}");
    ffi.Pointer<ffi.Float> valT = calloc(ffi.sizeOf<ffi.Float>() * 4);
    for (int i = 0; i < val.length; ++i) {
      valT[i] = val[i];
    }

    int ret = _addUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      UniformType.uniformVec4.index,
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  /// * add MAT2
  bool addMat2Uniform(String name, List<double> val) {
    assert(
        val.length == 4,
        "Assert error: mat2 has 4 doubles."
        "You have passed a list with ${val.length}");
    ffi.Pointer<ffi.Float> valT = calloc(ffi.sizeOf<ffi.Float>() * 4);
    for (int i = 0; i < val.length; ++i) {
      valT[i] = val[i];
    }

    int ret = _addUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      UniformType.uniformMat2.index,
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  /// * add MAT3
  bool addMat3Uniform(String name, List<double> val) {
    assert(
        val.length == 9,
        "Assert error: mat3 has 9 doubles."
        "You have passed a list with ${val.length}");
    ffi.Pointer<ffi.Float> valT = calloc(ffi.sizeOf<ffi.Float>() * 9);
    for (int i = 0; i < val.length; ++i) {
      valT[i] = val[i];
    }

    int ret = _addUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      UniformType.uniformMat3.index,
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  /// * add MAT4
  bool addMat4Uniform(String name, List<double> val) {
    assert(
        val.length == 16,
        "Assert error: mat4 has 16 doubles."
        "You have passed a list with ${val.length}");
    ffi.Pointer<ffi.Float> valT = calloc(ffi.sizeOf<ffi.Float>() * 16);
    for (int i = 0; i < val.length; ++i) {
      valT[i] = val[i];
    }

    int ret = _addUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      UniformType.uniformMat3.index,
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  late final _addUniformPtr = _lookup<
      ffi.NativeFunction<
          ffi.Int Function(ffi.Pointer<ffi.Char>, ffi.Int32,
              ffi.Pointer<ffi.Void>)>>('addUniform');
  late final _addUniform = _addUniformPtr.asFunction<
      int Function(ffi.Pointer<ffi.Char>, int, ffi.Pointer<ffi.Void>)>();

  /// ***********************************************
  /// **** REMOVE UNIFORM
  bool removeUniform(String name) {
    int ret = _removeUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
    );
    return ret == 0 ? false : true;
  }

  late final _removeUniformPtr =
      _lookup<ffi.NativeFunction<ffi.Int Function(ffi.Pointer<ffi.Char>)>>(
          'removeUniform');
  late final _removeUniform =
      _removeUniformPtr.asFunction<int Function(ffi.Pointer<ffi.Char>)>();

  /// ***********************************************
  /// **** ADD SAMPLER2D UNIFORM
  ///
  /// * add SAMPLER2D RGBA32
  bool addSampler2DUniform(
    String name,
    int width,
    int height,
    Uint8List val,
  ) {
    assert(
        val.length == width * height * 4,
        "\nAssert error: RGBA32 raw image length mismatch."
        "\nYou have passed a Uint8list with ${val.length}"
        "\nIt should be $width x $height * 4 = ${width * height * 4}");
    ffi.Pointer<ffi.Int8> valT = calloc(ffi.sizeOf<ffi.Int8>() * val.length);
    for (int i = 0; i < val.length; ++i) {
      valT[i] = val[i];
    }

    int ret = _addSampler2DUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      width,
      height,
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  late final _addSampler2DUniformPtr = _lookup<
      ffi.NativeFunction<
          ffi.Int Function(ffi.Pointer<ffi.Char>, ffi.Int32, ffi.Int32,
              ffi.Pointer<ffi.Void>)>>('addSampler2DUniform');
  late final _addSampler2DUniform = _addSampler2DUniformPtr.asFunction<
      int Function(ffi.Pointer<ffi.Char>, int, int, ffi.Pointer<ffi.Void>)>();

  /// ***********************************************
  /// **** REPLACE SAMPLER2D UNIFORM
  ///
  /// * replace SAMPLER2D texture with another one with different size
  bool replaceSampler2DUniform(
    String name,
    int width,
    int height,
    Uint8List val,
  ) {
    assert(
        val.length == width * height * 4,
        "\nAssert error: RGBA32 raw image length mismatch."
        "\nYou have passed a Uint8list with ${val.length}"
        "\nIt should be $width x $height * 4 = ${width * height * 4}");
    ffi.Pointer<ffi.Int8> valT = calloc(ffi.sizeOf<ffi.Int8>() * val.length);
    for (int i = 0; i < val.length; ++i) {
      valT[i] = val[i];
    }

    int ret = _replaceSampler2DUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      width,
      height,
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  late final _replaceSampler2DUniformPtr = _lookup<
      ffi.NativeFunction<
          ffi.Int Function(ffi.Pointer<ffi.Char>, ffi.Int32, ffi.Int32,
              ffi.Pointer<ffi.Void>)>>('replaceSampler2DUniform');
  late final _replaceSampler2DUniform = _replaceSampler2DUniformPtr.asFunction<
      int Function(ffi.Pointer<ffi.Char>, int, int, ffi.Pointer<ffi.Void>)>();

  /// ***********************************************
  /// **** SET UNIFORMs
  /// * set BOOL
  bool setBoolUniform(String name, bool val) {
    ffi.Pointer<ffi.Bool> valT = calloc(ffi.sizeOf<ffi.Bool>());
    valT.value = val;

    int ret = _setUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  /// * set INT
  bool setIntUniform(String name, int val) {
    ffi.Pointer<ffi.Int32> valT = calloc(ffi.sizeOf<ffi.Int32>());
    valT.value = val;

    int ret = _setUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  /// * set FLOAT
  bool setFloatUniform(String name, double val) {
    ffi.Pointer<ffi.Float> valT = calloc(ffi.sizeOf<ffi.Float>());
    valT.value = val;

    int ret = _setUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  /// * set VEC2
  bool setVec2Uniform(String name, List<double> val) {
    ffi.Pointer<ffi.Float> valT = calloc(ffi.sizeOf<ffi.Float>() * 2);
    valT[0] = val[0];
    valT[1] = val[1];

    int ret = _setUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  /// * set VEC3
  bool setVec3Uniform(String name, List<double> val) {
    ffi.Pointer<ffi.Float> valT = calloc(ffi.sizeOf<ffi.Float>() * 3);
    valT[0] = val[0];
    valT[1] = val[1];
    valT[2] = val[2];

    int ret = _setUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  /// * set VEC4
  bool setVec4Uniform(String name, List<double> val) {
    ffi.Pointer<ffi.Float> valT = calloc(ffi.sizeOf<ffi.Float>() * 4);
    valT[0] = val[0];
    valT[1] = val[1];
    valT[2] = val[2];
    valT[3] = val[3];

    int ret = _setUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  /// * set MAT2
  bool setMat2Uniform(String name, List<double> val) {
    ffi.Pointer<ffi.Float> valT = calloc(ffi.sizeOf<ffi.Float>() * 4);
    valT[0] = val[0];
    valT[1] = val[1];
    valT[2] = val[2];
    valT[3] = val[3];

    int ret = _setUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  /// * set MAT3
  bool setMat3Uniform(String name, List<double> val) {
    ffi.Pointer<ffi.Float> valT = calloc(ffi.sizeOf<ffi.Float>() * 9);
    valT[0] = val[0];
    valT[1] = val[1];
    valT[2] = val[2];
    valT[3] = val[3];
    valT[4] = val[4];
    valT[5] = val[5];
    valT[6] = val[6];
    valT[7] = val[7];
    valT[8] = val[8];

    int ret = _setUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  /// * set MAT4
  bool setMat4Uniform(String name, List<double> val) {
    ffi.Pointer<ffi.Float> valT = calloc(ffi.sizeOf<ffi.Float>() * 12);
    valT[0] = val[0];
    valT[1] = val[1];
    valT[2] = val[2];
    valT[3] = val[3];
    valT[4] = val[4];
    valT[5] = val[5];
    valT[6] = val[6];
    valT[7] = val[7];
    valT[8] = val[8];
    valT[9] = val[9];
    valT[10] = val[10];
    valT[11] = val[11];

    int ret = _setUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  /// * set SAMPLER2D
  ///
  /// Replace a texture with another image with the same size.
  /// Be sure the [val] length is the same as the previously
  /// stored image with int the uniform named [name].
  bool setSampler2DUniform(String name, Uint8List val) {
    ffi.Pointer<ffi.Int8> valT = calloc(ffi.sizeOf<ffi.Int8>() * val.length);
    for (int i = 0; i < val.length; ++i) {
      valT[i] = val[i];
    }

    int ret = _setUniform(
      name.toNativeUtf8().cast<ffi.Char>(),
      valT.cast<ffi.Void>(),
    );
    calloc.free(valT);
    return ret == 0 ? false : true;
  }

  late final _setUniformPtr = _lookup<
      ffi.NativeFunction<
          ffi.Int Function(
              ffi.Pointer<ffi.Char>, ffi.Pointer<ffi.Void>)>>('setUniform');
  late final _setUniform = _setUniformPtr
      .asFunction<int Function(ffi.Pointer<ffi.Char>, ffi.Pointer<ffi.Void>)>();
}
