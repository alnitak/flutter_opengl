import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'flutter_opengl_method_channel.dart';

abstract class FlutterOpenglPlatform extends PlatformInterface {
  /// Constructs a FlutterOpenglPlatform.
  FlutterOpenglPlatform() : super(token: _token);

  static final Object _token = Object();

  static FlutterOpenglPlatform _instance = MethodChannelFlutterOpengl();

  int textureId = -1;

  /// The default instance of [FlutterOpenglPlatform] to use.
  ///
  /// Defaults to [MethodChannelFlutterOpengl].
  static FlutterOpenglPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [FlutterOpenglPlatform] when
  /// they register themselves.
  static set instance(FlutterOpenglPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<int> createSurface(int width, int height) async {
    throw UnimplementedError('createSurface() has not been implemented.');
  }
}
