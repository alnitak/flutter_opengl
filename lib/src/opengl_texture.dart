import 'package:flutter/material.dart';
import 'package:flutter_opengl/src/opengl_controller.dart';

import 'flutter_opengl_ffi.dart';

/// A widget that contains the Texture widget.
///
/// The size of this widget is not related to the texture size.
/// It grabs also the mouse events to pass to the shader.
///
/// First get the id calling [OpenGLController().flutterOpenglPlugin.createSurface]
/// then feed this widget with id got:
///
/// '''dart
/// FutureBuilder<int>(
///   // get texture id
///   future: OpenGLController().openglPlugin.createSurface(
///         Utils.captured.size!.width.toInt(),
///         Utils.captured.size!.height.toInt(),
///       ),
///   builder: (context, textureId) {
///     if (!textureId.hasData || textureId.hasError) {
///       return const SizedBox.shrink();
///     }
///
///     // start renderer
///     OpenGLController().openglFFI.startThread();
///
///     // set your shader and texture here
///     OpenGLController().openglFFI.setShaderToy(
///       fragmentShaderString,
///     );
///
///     return SizedBox(
///       width: width,
///       height: height,
///       child: OpenGLTexture(
///         id: textureId,
///       ),
///     );
///   })
/// '''
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
