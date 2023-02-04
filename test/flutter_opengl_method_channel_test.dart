import 'package:flutter/services.dart';
import 'package:flutter_opengl/src/flutter_opengl_method_channel.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  MethodChannelFlutterOpengl platform = MethodChannelFlutterOpengl();
  const MethodChannel channel = MethodChannel('flutter_opengl_plugin');

  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return '42';
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

}
