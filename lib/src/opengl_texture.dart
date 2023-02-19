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
/// SizedBox(
///   width: 400,
///   height: 300,
///   child: FutureBuilder(
///     /// The surface size identifies the real texture size and
///     /// it is not related to the above SizedBox size
///     future: OpenGLController().openglPlugin.createSurface(300, 200),
///     builder: (_, snapshot) {
///       if (snapshot.hasError || !snapshot.hasData) {
///         return const SizedBox.shrink();
///       }
///       /// When the texture id is retrieved, it will be possible
///       /// to start the renderer, set a shader and display it.
///
///       /// Start renderer thread
///       OpenGLController().openglFFI.startThread();
///
///       /// Set the fragment shader
///       OpenGLController().openglFFI.setShaderToy(fShader);
///
///       /// build the texture widget
///       return OpenGLTexture(id: snapshot.data!);
///     },
///   ),
/// )
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
