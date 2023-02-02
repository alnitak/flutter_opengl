import 'package:flutter/material.dart';
import 'package:flutter_opengl/flutter_opengl.dart';
import 'package:image/image.dart' as img;

import 'shadertoy.dart';

/// Example widget that takes a child to be grabbed and used as
/// a texture to feed the given shader.
///
/// The renderer is activated by touching it, another touch
/// to interact with the shader and another touch to terminate the renderer
class ShaderWidget extends StatefulWidget {
  final Widget child;
  final int shaderToyIndex;

  const ShaderWidget({
    Key? key,
    required this.shaderToyIndex,
    required this.child,
  }) : super(key: key);

  @override
  State<ShaderWidget> createState() => _ShaderWidgetState();
}

class _ShaderWidgetState extends State<ShaderWidget> {
  GlobalKey childKey = GlobalKey();
  int nClicks = 0;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTapDown: (event) {
        if (nClicks == 0) {
          setState(() {});
        }
        nClicks++;
      },
      onTapUp: (event) {
        // stop renderer
        if (nClicks > 2) {
          OpenGLController().openglFFI.stopThread();
          setState(() {
            nClicks = 0;
          });
        }
      },
      child: Stack(
        children: [
          RepaintBoundary(
            key: childKey,
            child: widget.child,
          ),
          if (nClicks == 1)
            RendererWidget(
              childKeyToCapture: childKey,
              shaderToyIndex: widget.shaderToyIndex,
            ),
        ],
      ),
    );
  }
}

class RendererWidget extends StatelessWidget {
  final GlobalKey childKeyToCapture;
  final int shaderToyIndex;

  const RendererWidget({
    Key? key,
    required this.childKeyToCapture,
    required this.shaderToyIndex,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    // 1 - capture the widget
    // 2 - create OpenGL texture with the widget size
    // 3 - start renderer
    // 4 - set shader
    return FutureBuilder<CapturedWidget>(
      future: OGLUtils.captureWidget(childKeyToCapture),
      builder: (context, captured) {
        if (!captured.hasData ||
            captured.hasError ||
            ((captured.data?.size ?? Size.zero) == Size.zero)) {
          return const SizedBox.shrink();
        }
        // flip image data vertically
        img.Image flipped = img.Image.fromBytes(
          width: captured.data!.size.width.toInt(),
          height: captured.data!.size.height.toInt(),
          bytes: captured.data!.byteData.buffer,
          numChannels: 4,
        );
        flipped = img.flipVertical(flipped);

        return FutureBuilder<int>(
          // get texture id
          future: OpenGLController().openglPlugin.createSurface(
                captured.data!.size.width.toInt(),
                captured.data!.size.height.toInt(),
              ),
          builder: (context, textureId) {
            if (!textureId.hasData || textureId.hasError) {
              return const SizedBox.shrink();
            }
            // set the shader
            OpenGLController().openglFFI.setShaderToy(
                  shaderToyTexture[shaderToyIndex]['fragment']!,
                );

            // start renderer
            OpenGLController().openglFFI.startThread();

            // Seems that on Windows the textures must be sent after
            // Texture() widget has been drawn?
            Future.delayed(const Duration(milliseconds: 0), () {
              // add the grabbed widget as texture on iChannel0 uniform
              OpenGLController().openglFFI.addSampler2DUniform(
                    'iChannel0',
                    captured.data!.size.width.toInt(),
                    captured.data!.size.height.toInt(),
                    flipped.getBytes(order: img.ChannelOrder.rgba),
                  );

              // set the 2nd texture uniform
              OGLUtils.setAssetTexture('iChannel1', 'assets/texture.png');
            });

            return SizedBox(
              width: captured.data!.size.width,
              height: captured.data!.size.height,
              child: OpenGLTexture(
                id: textureId.data!,
              ),
            );
          },
        );
      },
    );
  }
}
