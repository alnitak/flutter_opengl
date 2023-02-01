import 'dart:ffi' as ffi;
import 'dart:io';

import 'flutter_opengl.dart';
import 'flutter_opengl_ffi.dart';

/// Controller that expose method channel and FFI
class OpenGLController {
  static OpenGLController? _instance;

  factory OpenGLController() => _instance ??= OpenGLController._();

  OpenGLController._();

  late ffi.DynamicLibrary nativeLib;
  late final FlutterOpengl openglPlugin;
  late final FlutterOpenGLFfi openglFFI;

  initializeGL() {
    nativeLib = Platform.isAndroid
        ? ffi.DynamicLibrary.open("libflutter_opengl_plugin.so")
        : (Platform.isWindows
            ? ffi.DynamicLibrary.open("flutter_opengl_plugin.dll")
            : ffi.DynamicLibrary.process());
    openglFFI = FlutterOpenGLFfi.fromLookup(nativeLib.lookup);
    openglPlugin = FlutterOpengl();
  }
}
