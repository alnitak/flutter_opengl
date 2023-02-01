import 'flutter_opengl_platform_interface.dart';

class FlutterOpengl {
  /// This function needs to call native to prepare the texture
  /// All the other methods use FFI
  Future<int> createSurface(int width, int height) {
    return FlutterOpenglPlatform.instance.createSurface(width, height);
  }
}
