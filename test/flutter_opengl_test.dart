import 'package:flutter_opengl/src/flutter_opengl_method_channel.dart';
import 'package:flutter_opengl/src/flutter_opengl_platform_interface.dart';
import 'package:flutter_test/flutter_test.dart';

// class MockFlutterOpenglPlatform
//     with MockPlatformInterfaceMixin
//     implements FlutterOpenglPlatform {
//
//   @override
//   Future<String?> getPlatformVersion() => Future.value('42');
// }

void main() {
  final FlutterOpenglPlatform initialPlatform = FlutterOpenglPlatform.instance;

  test('$MethodChannelFlutterOpengl is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelFlutterOpengl>());
  });

  // test('getPlatformVersion', () async {
  //   FlutterOpengl flutterOpenglPlugin = FlutterOpengl();
  //   MockFlutterOpenglPlatform fakePlatform = MockFlutterOpenglPlatform();
  //   FlutterOpenglPlatform.instance = fakePlatform;
  //
  //   expect(await flutterOpenglPlugin.getPlatformVersion(), '42');
  // });
}
