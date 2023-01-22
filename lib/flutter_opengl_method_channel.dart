import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'flutter_opengl_platform_interface.dart';

/// An implementation of [FlutterOpenglPlatform] that uses method channels.
class MethodChannelFlutterOpengl extends FlutterOpenglPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('flutter_opengl_plugin');

  @override
  Future<int> createSurface(int width, int height) async {
    int? textureId;
    try {
      textureId = await methodChannel.invokeMethod<int>('createSurface', {
        'width': width,
        'height': height,
      });
    } on PlatformException catch (e) {
      debugPrint(e.toString());
      return -1;
    }
    return textureId ?? -1;
  }

  @override
  Future draw() async {
    try {
      await methodChannel.invokeMethod('draw');
      return;
    } on PlatformException catch (e) {
      debugPrint(e.toString());
    }
    return Future.value();
  }
}
