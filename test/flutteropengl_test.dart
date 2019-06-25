import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:flutteropengl/flutteropengl.dart';

void main() {
  const MethodChannel channel = MethodChannel('flutteropengl');

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return '42';
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

  test('getPlatformVersion', () async {
//    expect(await Flutteropengl.platformVersion, '42');
  });
}
