import 'dart:async';
import 'dart:ui' as ui;

import 'package:flutter/material.dart';
import 'package:flutter/rendering.dart';
import 'package:flutter/services.dart';
import 'package:image/image.dart' as img;
import 'package:image/image.dart';

import 'opengl_controller.dart';

/// class to store captured widget
class CapturedWidget {
  ByteData byteData; // uncompressed 32bit RGBA image data
  Size size;

  CapturedWidget(this.byteData, this.size) {}
}

class OGLUtils {
  /// Capture raw RGBA32 data image
  static Future<CapturedWidget> captureWidget(GlobalKey widgetKey) async {
    final RenderRepaintBoundary boundary =
        widgetKey.currentContext!.findRenderObject()! as RenderRepaintBoundary;
    final ui.Image image = await boundary.toImage(pixelRatio: 1.0);
    final ByteData? byteData =
        await image.toByteData(format: ui.ImageByteFormat.rawRgba);

    if (byteData == null) {
      return CapturedWidget(ByteData(0), Size.zero);
    }

    return CapturedWidget(
      byteData,
      Size(image.width.toDouble(), image.height.toDouble()),
    );
  }

  /// Load an asset image, flip vertically and
  /// add it to the shader uniform with the name [uniformName]
  static setAssetTexture(String uniformName, String assetName,
      {bool toCreate = false}) async {
    final Uint8List inputImg =
        (await rootBundle.load(assetName)).buffer.asUint8List();
    final decoder = img.PngDecoder();
    img.Image? decodedImg = decoder.decode(inputImg);

    img.Image rgba = decodedImg!.convert(
      format: Format.uint8,
      numChannels: 4,
      withPalette: false,
    );

    // texture in OpenGL have the origin at top left, so flip it vertically
    rgba = img.flipVertical(rgba);

    final decodedBytes = rgba.getBytes(order: img.ChannelOrder.rgba);

    if (toCreate) {
      OpenGLController()
          .openglFFI
          .setSampler2DUniform(uniformName, decodedBytes);
    } else {
      OpenGLController().openglFFI.addSampler2DUniform(
          uniformName, rgba.width, rgba.height, decodedBytes);
    }
  }
}
