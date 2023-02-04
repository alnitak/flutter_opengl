import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter_opengl/flutter_opengl.dart';
import 'package:flutter_opengl_example/shadertoy.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

import 'states.dart';

/// Tab page to test the plugin
/// - create the texture id and use it in the Texture() widget
/// - start/stop renderer
/// - choose shader samples
class Controls extends ConsumerWidget {

   Controls({
    Key? key,
  }) : super(key: key);

  Timer? fpsTimer;

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final textureCreated = ref.watch(stateTextureCreated);

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
                Size textureSize = ref.read(stateTextureSize);
                int id = await OpenGLController().openglPlugin.createSurface(
                  textureSize.width.toInt(),
                  textureSize.height.toInt(),
                    );
                  ref.read(stateTextureCreated.notifier).state =
                      OpenGLController().openglFFI.rendererStatus();
                ref.read(stateTextureId.notifier).state = id;
              },
              child: const Text('create texture'),
            ),

            const SizedBox(width: 16),


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
                    fpsTimer =
                        Timer.periodic(const Duration(seconds: 1), (timer) {
                          double fps = OpenGLController().openglFFI.getFps();
                          ref.read(stateFPS.notifier).state = '${fps.toStringAsFixed(1)} FPS';
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
                      ref.read(stateTextureCreated.notifier).state =false;
                    ref.read(stateShaderButtonId.notifier).state = -1;
                    ref.read(stateShaderTextureButtonId.notifier).state = -1;
                  },
                  child: const Text('stop'),
                ),
              ],
            ),
          ],
        ),
        Row(
          children: [
            /// iChannel0
            ElevatedButton(
              onPressed: () async {
                // OGLUtils.setAssetTexture('iChannel0', 'assets/texture.png');
                OGLUtils.setAssetTexture(
                    'iChannel0', 'assets/dash.png');
              },
              child: const Text('iChannel0'),
            ),

            // /// iChannel1
            // ElevatedButton(
            //   onPressed: () async {
            //     OGLUtils.setAssetTexture('iChannel1', 'assets/dash.png');
            //   },
            //   child: const Text('iChannel1'),
            // ),

            /// 1
            ElevatedButton(
              onPressed: () async {
                OGLUtils.setAssetTexture(
                    'iChannel0', 'assets/rgba-noise-medium.png',
                    toCreate: true);
              },
              child: const Text('1'),
            ),

            /// 2
            ElevatedButton(
              onPressed: () async {
                OGLUtils.setAssetTexture(
                    'iChannel0', 'assets/dash.png',
                    toCreate: true);
              },
              child: const Text('2'),
            ),
          ],
        ),
        const SizedBox(height: 10),

        /// SET TEXTURE SIZE
        const TextureSize(),

        /// SHADERS BUTTONS
        const ShaderButtons(),
      ],
    );
  }
}

/// Shader buttons (without texture)
class ShaderButtons extends ConsumerWidget {
  const ShaderButtons({
    Key? key,
  }) : super(key: key);

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final activeButtonId = ref.watch(stateShaderButtonId);

    return Expanded(
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
                  bool hasIChannel0 = shaderToy[i]['fragment']!.contains('iChannel0');
                  bool hasIChannel1 = shaderToy[i]['fragment']!.contains('iChannel1');
                  return ElevatedButton(
                    onPressed: () {
                      ref.read(stateUrl.notifier).state = shaderToy[i]['url']!;
                      OpenGLController().openglFFI.setShaderToy(
                        shaderToy[i]['fragment']!,
                      );
                      Size size =
                      OpenGLController().openglFFI.getTextureSize();
                      if (size.width != -1) {
                        ref.read(stateShaderButtonId.notifier).state = i;
                        ref.read(stateShaderTextureButtonId.notifier).state = -1;
                      } else {
                        ref.read(stateShaderButtonId.notifier).state = -1;
                      }
                    },
                    style: ButtonStyle(
                      fixedSize:
                      const MaterialStatePropertyAll(Size(10, 10)),
                      backgroundColor:
                      i == activeButtonId
                          ? const MaterialStatePropertyAll(Colors.green)
                          : null,
                    ),
                    child: Column(
                      mainAxisSize: MainAxisSize.min,
                      mainAxisAlignment: MainAxisAlignment.center,
                      children: [
                        Text('${i + 1}'),
                        if (hasIChannel0 || hasIChannel1)
                          Row(
                            children: [
                              if (hasIChannel0) const Icon(Icons.image, size: 16),
                              if (hasIChannel1) const Icon(Icons.image, size: 16),
                            ],
                          ),
                      ],
                    ),
                  );
                }),
              ],
            ),


          ],
        ),
      ),
    );
  }
}

class TextureSize extends ConsumerWidget {
  const TextureSize({
    Key? key,
  }) : super(key: key);

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final textureSizes = [
      const Size(150, 84),
      const Size(600, 337),
      const Size(1000, 563),
    ];
    final textureSize = ref.watch(stateTextureSize);

    return Wrap(
      runSpacing: -25,
      children: List.generate(textureSizes.length, (n) {
        return SizedBox(
          height: 60,
          child: FittedBox(
            child: SizedBox(
              height: 80,
              child: Row(
                children: [
                  Checkbox(
                    value: textureSize == textureSizes[n],
                    onChanged: (val) {
                      ref.read(stateTextureSize.notifier).state = textureSizes[n];
                    },
                  ),
                  Text(
                    '${textureSize.width.toInt()} x '
                        '${textureSize.height.toInt()}',
                    textScaleFactor: 1.0,
                  ),
                ],
              ),
            ),
          ),
        );
      }),
    );
  }
}