import 'package:flutter/gestures.dart';
import 'package:flutter/material.dart';
import 'package:flutter_opengl/flutter_opengl.dart';
import 'package:flutter_opengl_example/controls.dart';
import 'package:flutter_opengl_example/edit_shader.dart';
import 'package:url_launcher/url_launcher.dart';

import 'test_widget.dart';

void main() {
  OpenGLController().initializeGL();
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  late ValueNotifier<String> shaderUrl;
  late ValueNotifier<String> fpsText;
  late ValueNotifier<int> textureId;
  late ValueNotifier<Size> textureSize;

  @override
  void initState() {
    super.initState();

    textureId = ValueNotifier(-1);
    fpsText = ValueNotifier('');
    shaderUrl = ValueNotifier('');

    // textureSize = ValueNotifier(const Size(300, 168));
    textureSize = ValueNotifier(const Size(600, 337));
    // textureSize = ValueNotifier(const Size(1000, 563));
    // textureSize = ValueNotifier(const Size(1500, 844));
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      theme: ThemeData(
        primarySwatch: Colors.blue,
        brightness: Brightness.dark,
      ),
      home: DefaultTabController(
        length: 4,
        child: Scaffold(
          body: Padding(
            padding: const EdgeInsets.all(8.0),
            child: Column(
              mainAxisSize: MainAxisSize.max,
              children: [
                /// FPS and ShaderToy URL text
                ValueListenableBuilder<String>(
                    valueListenable: shaderUrl,
                    builder: (_, shaderUrl, __) {
                      return ValueListenableBuilder<String>(
                          valueListenable: fpsText,
                          builder: (_, fps, __) {
                            return Row(
                              mainAxisSize: MainAxisSize.min,
                              children: [
                                Text(
                                    '$fps\n${textureSize.value.width.toInt()} x '
                                    '${textureSize.value.height.toInt()}',
                                    textScaleFactor: 1.3),
                                const SizedBox(width: 30),
                                RichText(
                                  text: TextSpan(
                                    children: [
                                      TextSpan(
                                        text: shaderUrl,
                                        style: const TextStyle(
                                            decoration:
                                                TextDecoration.underline,
                                            fontWeight: FontWeight.bold),
                                        recognizer: TapGestureRecognizer()
                                          ..onTap = () {
                                            launchUrl(Uri.parse(shaderUrl));
                                          },
                                      ),
                                    ],
                                  ),
                                ),
                              ],
                            );
                          });
                    }),
                const SizedBox(height: 8),

                /// TEXTURE
                AspectRatio(
                  aspectRatio:
                      textureSize.value.width / textureSize.value.height,
                  child: ValueListenableBuilder<int>(
                    valueListenable: textureId,
                    builder: (_, id, __) {
                      if (id == -1) {
                        return Container(
                          color: Colors.red,
                        );
                      }
                      return OpenGLTexture(
                        id: id,
                      );
                    },
                  ),
                ),

                const SizedBox(
                  height: 40,
                  child: TabBar(
                    tabs: [
                      Tab(text: 'controls'),
                      Tab(text: 'edit shader'),
                      Tab(text: 'test 1'),
                      Tab(text: 'test 2'),
                    ],
                  ),
                ),

                const SizedBox(height: 12),

                /// TABS
                Expanded(
                  child: TabBarView(
                    physics: NeverScrollableScrollPhysics(),
                    children: [
                      Controls(
                        onFPSChanged: (newFPS) => fpsText.value = newFPS,
                        onUrlChanged: (newUrl) => shaderUrl.value = newUrl,
                        onTextureIdChanged: (id) => textureId.value = id,
                        onTextureSizeChanged: (size) =>
                            textureSize.value = size,
                      ),
                      ValueListenableBuilder<String>(
                        valueListenable: shaderUrl,
                        builder: (_, shaderUrl, __) {
                          return const EditShader();
                        },
                      ),
                      const TestWidget(shaderToyCode: 'ls3cDB'),
                      const TestWidget(shaderToyCode: 'XdXGR7'),
                    ],
                  ),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}
