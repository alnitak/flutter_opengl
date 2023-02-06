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
                  Size size = OpenGLController().openglFFI.getTextureSize();
                  if (size.width != -1) {
                    ref.read(stateShaderIndex.notifier).state = i;
                  } else {
                    ref.read(stateShaderIndex.notifier).state = -1;
                  }
                  ref.read(stateChannel0.notifier).state =
                      TextureParams().copyWith(assetsImage: '');
                  ref.read(stateChannel1.notifier).state =
                      TextureParams().copyWith(assetsImage: '');
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
                    if (hasIChannel0 ||
                        hasIChannel1 ||
                        hasIChannel2 ||
                        hasIChannel3)
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
