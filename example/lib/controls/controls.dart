import 'dart:async';

import 'package:file_picker/file_picker.dart';
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

              const SizedBox(width: 16),

              /// PICK VIDEO FILE
              ElevatedButton(
                onPressed: () async {
                  FilePickerResult? result =
                      await FilePicker.platform.pickFiles(
                        type: FileType.video,
                      );

                  if (result != null) {
                    ref.read(statePickedVideo.notifier).state =
                        result.files.single.path!;
                  } else {
                    // User canceled the picker
                  }
                },
                child: const Text('pick a\nvideo'),
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
