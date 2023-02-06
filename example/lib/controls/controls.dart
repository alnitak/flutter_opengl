import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter_opengl/flutter_opengl.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../states.dart';
import 'ShaderButtons.dart';
import 'texture_chooser.dart';
import 'texture_sizes.dart';

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

    return SingleChildScrollView(
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          /// CREATE TEXTURE
          Row(
            mainAxisAlignment: MainAxisAlignment.center,
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
                        ref.read(stateFPS.notifier).state = fps;
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
                      ref.read(stateTextureCreated.notifier).state = false;
                      ref.read(stateShaderIndex.notifier).state = -1;
                    },
                    child: const Text('stop'),
                  ),
                ],
              ),
            ],
          ),
          const SizedBox(height: 10),

          /// SET TEXTURE SIZE
          const TextureSize(),

          /// SHADERS BUTTONS
          const ShaderButtons(),

          const SizedBox(height: 10),

          /// CHOOSE TEXTURE
          const TextureChooser(),
        ],
      ),
    );
  }
}




