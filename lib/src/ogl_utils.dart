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

  CapturedWidget(this.byteData, this.size);
}

enum AddMethod {
  /// Add Sampler2D uniform if not already exists
  add,

  /// Replace Sampler2D uniform with another with different size
  replace,

  /// Set a new Sampler2D uniform with the same size
  set,
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
  /// [toReplace] if true replace the texture with another
  /// one with different size.
  static Future<bool> setAssetTexture(String uniformName, String assetName,
      {AddMethod method = AddMethod.replace}) async {
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

    bool ret = false;
    switch (method) {
      case AddMethod.add:
        ret = OpenGLController().openglFFI.addSampler2DUniform(
            uniformName, rgba.width, rgba.height, decodedBytes);
        break;
      case AddMethod.replace:
        ret = OpenGLController().openglFFI.replaceSampler2DUniform(
            uniformName, rgba.width, rgba.height, decodedBytes);
        break;
      case AddMethod.set:
        ret = OpenGLController()
            .openglFFI
            .setSampler2DUniform(uniformName, decodedBytes);
        break;
    }
    return ret;
  }
}
