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
    return Listener(
      onPointerDown: (event) {
        OpenGLController().openglFFI.setMousePosition(
            event.localPosition, PointerEventType.onPointerDown);
      },
      onPointerMove: (event) {
        OpenGLController().openglFFI.setMousePosition(
            event.localPosition, PointerEventType.onPointerMove);
      },
      onPointerUp: (event) {
        OpenGLController().openglFFI.setMousePosition(
            event.localPosition, PointerEventType.onPointerUp);
      },
      child: Container(
        color: Colors.black,
        child: Texture(textureId: id),
      ),
    );
  }
}
