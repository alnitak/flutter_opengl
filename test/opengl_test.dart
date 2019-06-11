import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:opengl/opengl.dart';

void main() {
  const MethodChannel channel = MethodChannel('opengl');

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return '42';
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

  test('getPlatformVersion', () async {
//    expect(await Opengl.platformVersion, '42');
  });
}
