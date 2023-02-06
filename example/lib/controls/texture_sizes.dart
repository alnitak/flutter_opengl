
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

import '../states.dart';

/// Texture radio button sizes to choose
///
class TextureSize extends ConsumerWidget {
  const TextureSize({
    Key? key,
  }) : super(key: key);

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final textureSizes = [
      const Size(150, 84),
      const Size(300, 170),
      const Size(600, 338),
      const Size(1200, 676),
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
                      ref.read(stateTextureSize.notifier).state =
                      textureSizes[n];
                    },
                  ),
                  Text(
                    '${textureSizes[n].width.toInt()} x '
                        '${textureSizes[n].height.toInt()}',
                    textScaleFactor: 1.0,
                  ),
                  const SizedBox(width: 6),
                ],
              ),
            ),
          ),
        );
      }),
    );
  }
}
