import 'dart:io';
import 'dart:typed_data';

import 'package:file_picker/file_picker.dart';
import 'package:flutter/material.dart';
import 'package:flutter_opengl/flutter_opengl.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../shadertoy.dart';
import '../states.dart';

/// Shader buttons (without texture)
///
class ShaderButtons extends ConsumerWidget {
  const ShaderButtons({
    Key? key,
  }) : super(key: key);

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final activeButtonId = ref.watch(stateShaderIndex);

    return Column(
      mainAxisSize: MainAxisSize.min,
      children: [
        const Text('Shader examples'),
        Wrap(
          alignment: WrapAlignment.center,
          runSpacing: 4,
          spacing: 4,
          children: [

            /// Build button for each fragments stored in [shaderToy] list
            /// Each button display also if a iChannelN is present
            /// displaying the iChannel number below the button number
            ...List.generate(shaderToy.length, (i) {
              bool hasIChannel0 =
                  shaderToy[i]['fragment']!.contains('iChannel0');
              bool hasIChannel1 =
                  shaderToy[i]['fragment']!.contains('iChannel1');
              bool hasIChannel2 =
                  shaderToy[i]['fragment']!.contains('iChannel2');
              bool hasIChannel3 =
                  shaderToy[i]['fragment']!.contains('iChannel3');
              return ElevatedButton(
                onPressed: () {
                  ref.read(stateUrl.notifier).state = shaderToy[i]['url']!;
                  OpenGLController().openglFFI.setShaderToy(
                        shaderToy[i]['fragment']!,
                      );
                  // Size size = OpenGLController().openglFFI.getTextureSize();
                  ref.read(stateShaderIndex.notifier).state = i;

                  /// reset bottom TextureChooser
                  ref.read(stateChannel0.notifier).state =
                      TextureParams().copyWith(assetsImage: '');
                  ref.read(stateChannel1.notifier).state =
                      TextureParams().copyWith(assetsImage: '');
                  ref.read(stateChannel2.notifier).state =
                      TextureParams().copyWith(assetsImage: '');
                  ref.read(stateChannel3.notifier).state =
                      TextureParams().copyWith(assetsImage: '');
                  /// stop capturing
                  if (ref.read(stateCaptureRunning)) {
                    OpenGLController().openglFFI.stopCapture();
                    ref.read(stateCaptureRunning.notifier).state = false;
                  }
                },
                style: ButtonStyle(
                  fixedSize: const MaterialStatePropertyAll(Size(65, 45)),
                  backgroundColor: i == activeButtonId
                      ? const MaterialStatePropertyAll(Colors.green)
                      : null,
                ),
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    Text('${i + 1}'),
                      Row(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          if (hasIChannel0)
                            const Text('0 ', textScaleFactor: 0.8),
                          if (hasIChannel1)
                            const Text('1 ', textScaleFactor: 0.8),
                          if (hasIChannel2)
                            const Text('2 ', textScaleFactor: 0.8),
                          if (hasIChannel3)
                            const Text('3', textScaleFactor: 0.8),
                        ],
                      ),
                  ],
                ),
              );
            }),
          ],
        ),
      ],
    );
  }
}
