import 'package:flutter/material.dart';
import 'package:flutter/rendering.dart';
import 'package:flutter_opengl/opengl_controller.dart';
import 'package:flutter_opengl/opengl_texture.dart';
import 'package:image/image.dart' as img;

import 'Utils.dart';
import 'shadertoy.dart';

class ShaderWidget extends StatefulWidget {
  final Widget child;
  final int shaderToyIndex;

  ShaderWidget({
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
    return FutureBuilder<bool>(
      future: Utils.captureWidget(childKeyToCapture),
      builder: (context, screenshot) {
        if (!screenshot.hasData ||
            screenshot.hasError ||
            !(screenshot.data ?? false)) {
          return const SizedBox.shrink();
        }
        print('FINAL WIDGET SIZE: ${Utils.captured.size}');
        // flip capture data vertically
        img.Image flipped = img.Image.fromBytes(
          width: Utils.captured.size!.width.toInt(),
          height: Utils.captured.size!.height.toInt(),
          bytes: Utils.captured.byteData!.buffer,
          numChannels: 4,
        );
        flipped = img.flipVertical(flipped);

        return FutureBuilder<int>(
          // get texture id
          future: OpenGLController().openglPlugin.createSurface(
                Utils.captured.size!.width.toInt(),
                Utils.captured.size!.height.toInt(),
              ),
          builder: (context, textureId) {
            if (!textureId.hasData || textureId.hasError) {
              return const SizedBox.shrink();
            }
            // set the shader
            OpenGLController().openglFFI.setShaderToy(
                  shadertoy[shaderToyIndex]['fragment']!,
                );

            // start renderer
            OpenGLController().openglFFI.startThread();

            // add texture
            OpenGLController().openglFFI.addSampler2DUniform(
                  'iChannel0',
                  Utils.captured.size!.width.toInt(),
                  Utils.captured.size!.height.toInt(),
                  flipped.getBytes(order: img.ChannelOrder.rgba),
                );

            Utils.setAssetTexture('iChannel1', 'assets/texture.png');

            return Container(
              color: Colors.yellow.withOpacity(0.1),
              width: Utils.captured.size!.width,
              height: Utils.captured.size!.height,
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
