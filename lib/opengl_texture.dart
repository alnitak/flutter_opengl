import 'package:flutter/material.dart';

import 'flutter_opengl_ffi.dart';
import 'opengl_controller.dart';

/// A widget that contains the Texture widget.
///
/// The size of this widget is not related to the texture
/// size use when calling [OpenGLController().openglPlugin.createSurface()]
class OpenGLTexture extends StatelessWidget {
  /// the texture id got by
  /// [OpenGLController().flutterOpenglPlugin.createSurface]
  final int id;

  const OpenGLTexture({
    Key? key,
    required this.id,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    Size twSize = Size.zero;
    Offset startingPos = Offset.zero;
    var key = GlobalKey();

    return Listener(
      onPointerDown: (event) {
        startingPos = event.localPosition;
        OpenGLController().openglFFI.setMousePosition(
              startingPos,
              event.localPosition,
              PointerEventType.onPointerDown,
              twSize,
            );
      },
      onPointerMove: (event) {
        OpenGLController().openglFFI.setMousePosition(
              startingPos,
              event.localPosition,
              PointerEventType.onPointerMove,
              twSize,
            );
      },
      onPointerUp: (event) {
        OpenGLController().openglFFI.setMousePosition(
              startingPos,
              event.localPosition,
              PointerEventType.onPointerUp,
              twSize,
            );
      },
      child: LayoutBuilder(builder: (_, __) {
        WidgetsBinding.instance.addPostFrameCallback((timeStamp) {
          final box = context.findRenderObject() as RenderBox;
          twSize = box.size;
        });

        return ColoredBox(
          key: key,
          color: Colors.black,
          child: Texture(textureId: id),
        );
      }),
    );
  }
}
