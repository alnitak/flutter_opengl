import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_opengl/flutter_opengl.dart';
import 'package:flutter_opengl_example/shadertoy.dart';

bool textureCreated = false;
bool shaderActive = false;
int currentShaderIndex = -1;
int currentShaderTextureIndex = -1;

/// Tab page to test the plugin
/// - create the texture id and use it in the Texture() widget
/// - start/stop renderer
/// - choose shader samples
class Controls extends StatefulWidget {
  final Function(Size) onTextureSizeChanged;
  final Function(int) onTextureIdChanged;
  final Function(String) onFPSChanged;
  final Function(String) onUrlChanged;

  const Controls({
    Key? key,
    required this.onTextureSizeChanged,
    required this.onTextureIdChanged,
    required this.onFPSChanged,
    required this.onUrlChanged,
  }) : super(key: key);

  @override
  State<Controls> createState() => _ControlsState();
}

class _ControlsState extends State<Controls> {
  Timer? fpsTimer;
  late ValueNotifier<int> indexTextureSize;
  final textureSizes = [
    const Size(150, 84),
    const Size(600, 337),
    const Size(1000, 563),
  ];

  @override
  void initState() {
    super.initState();
    indexTextureSize = ValueNotifier(1);
    widget.onTextureSizeChanged(textureSizes[1]);
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      mainAxisSize: MainAxisSize.min,
      children: [
        /// CREATE TEXTURE
        Row(
          children: [
            ElevatedButton(
              style: ButtonStyle(
                backgroundColor: textureCreated
                    ? const MaterialStatePropertyAll(Colors.green)
                    : const MaterialStatePropertyAll(Colors.red),
              ),
              onPressed: () async {
                fpsTimer?.cancel();
                int id = await OpenGLController().openglPlugin.createSurface(
                      textureSizes[indexTextureSize.value].width.toInt(),
                      textureSizes[indexTextureSize.value].height.toInt(),
                    );
                setState(() {
                  textureCreated =
                      OpenGLController().openglFFI.rendererStatus();
                  shaderActive = false;
                });
                widget.onTextureIdChanged(id);
              },
              child: const Text('create texture'),
            ),

            const SizedBox(width: 16),

            /// iChannel0
            ElevatedButton(
              onPressed: () async {
                OGLUtils.setAssetTexture('iChannel0', 'assets/texture.png');
              },
              child: const Text('iChannel0'),
            ),

            /// iChannel1
            ElevatedButton(
              onPressed: () async {
                OGLUtils.setAssetTexture('iChannel1', 'assets/dash.png');
              },
              child: const Text('iChannel1'),
            ),
          ],
        ),
        const SizedBox(height: 10),

        /// SET TEXTURE SIZE
        Wrap(
          runSpacing: -25,
          children: List.generate(textureSizes.length, (n) {
            return ValueListenableBuilder<int>(
                valueListenable: indexTextureSize,
                builder: (_, i, __) {
                  return SizedBox(
                    height: 60,
                    child: FittedBox(
                      child: SizedBox(
                        width: 130,
                        height: 80,
                        child: Row(
                          children: [
                            Checkbox(
                              value: i == n,
                              onChanged: (val) {
                                indexTextureSize.value = n;
                                widget.onTextureSizeChanged(textureSizes[n]);
                              },
                            ),
                            Text(
                              '${textureSizes[n].width.toInt()} x '
                              '${textureSizes[n].height.toInt()}',
                              textScaleFactor: 1.0,
                            ),
                          ],
                        ),
                      ),
                    ),
                  );
                });
          }),
        ),

        /// SHADERS BUTTONS
        Expanded(
          child: SingleChildScrollView(
            child: Column(
              mainAxisSize: MainAxisSize.min,
              children: [
                const Text('Shader examples'),
                Wrap(
                  alignment: WrapAlignment.center,
                  runSpacing: 4,
                  spacing: 4,
                  children: [
                    // ShaderToy, provide only fragment source
                    ...List.generate(shaderToy.length, (i) {
                      return ElevatedButton(
                        onPressed: () {
                          widget.onUrlChanged(shaderToy[i]['url']!);
                          OpenGLController().openglFFI.setShaderToy(
                            shaderToy[i]['fragment']!,
                          );
                          Size size = OpenGLController().openglFFI.getTextureSize();
                          if (size.width != -1) {
                            currentShaderTextureIndex = -1;
                            currentShaderIndex = i;
                            shaderActive = true;
                          } else {
                            currentShaderIndex = -1;
                          }
                          setState(() {});
                        },
                        style: ButtonStyle(
                          fixedSize: const MaterialStatePropertyAll(Size(10,10)),
                          backgroundColor: shaderActive && i == currentShaderIndex
                              ? const MaterialStatePropertyAll(Colors.green)
                              : null,
                        ),
                        child: Text('${i + 1}'),
                      );
                    }),
                  ],
                ),

                /// SHADERS BUTTONS with textures
                const SizedBox(height: 10),
                const Text('Shader examples with textures'),
                Wrap(
                  alignment: WrapAlignment.center,
                  runSpacing: 4,
                  spacing: 4,
                  children: [
                    // ShaderToy, provide only fragment source
                    ...List.generate(shaderToyTexture.length, (i) {
                      return ElevatedButton(
                        onPressed: () {
                          widget.onUrlChanged(shaderToyTexture[i]['url']!);
                          OpenGLController().openglFFI.setShaderToy(
                            shaderToyTexture[i]['fragment']!,
                          );
                          Size size = OpenGLController().openglFFI.getTextureSize();
                          if (size.width != -1) {
                            currentShaderIndex = -1;
                            currentShaderTextureIndex = i;
                            shaderActive = true;
                          } else {
                            currentShaderTextureIndex = -1;
                          }
                          setState(() {});
                        },
                        style: ButtonStyle(
                          backgroundColor: shaderActive && i == currentShaderTextureIndex
                              ? const MaterialStatePropertyAll(Colors.green)
                              : null,
                        ),
                        child: Text('${i + 1}'),
                      );
                    }),
                  ],
                ),

                const SizedBox(height: 10),

                /// START STOP
                Row(
                  mainAxisSize: MainAxisSize.max,
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    /// START
                    ElevatedButton(
                      onPressed: () {
                        OpenGLController().openglFFI.startThread();
                        fpsTimer?.cancel();
                        fpsTimer = Timer.periodic(const Duration(seconds: 1), (timer) {
                          double fps = OpenGLController().openglFFI.getFps();
                          widget.onFPSChanged('${fps.toStringAsFixed(1)} FPS');
                        });
                      },
                      child: const Text('start'),
                    ),
                    const SizedBox(width: 8),

                    /// STOP
                    ElevatedButton(
                      onPressed: () {
                        fpsTimer?.cancel();
                        OpenGLController().openglFFI.stopThread();
                        setState(() {
                          textureCreated = false;
                          shaderActive = false;
                          currentShaderIndex = -1;
                        });
                      },
                      child: const Text('stop'),
                    ),
                  ],
                ),

              ],
            ),
          ),
        ),

      ],
    );
  }
}
