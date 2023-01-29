import 'dart:async';
import 'dart:typed_data';
import 'dart:ui' as ui;

import 'package:flutter/material.dart';
import 'package:flutter/rendering.dart';
import 'package:flutter/services.dart';
import 'package:flutter_opengl/opengl_controller.dart';
import 'package:image/image.dart' as img;
import 'package:image/image.dart';


/// class to store captured widget
class CapturedWidget {
  ByteData? byteData; // uncompressed 32bit RGBA image data
  Size? size;

  CapturedWidget(this.byteData, this.size) {}
}

class Utils {

  /// Load an asset image, flip vertically and
  /// add it to the shader uniform with the name [uniformName]
  static setAssetTexture(String uniformName, String assetName) async {
    final Uint8List inputImg = (await rootBundle.load(assetName)).buffer.asUint8List();
    final decoder = img.PngDecoder();
    img.Image? decodedImg = decoder.decode(inputImg);

    img.Image rgba = decodedImg!.convert(
      format: Format.uint8,
      numChannels: 4,
      withPalette: false,
    );

    // texture in OpenGL have the origin at top left, so flip vertically
    rgba = img.flipVertical(rgba);

    final decodedBytes = rgba.getBytes(order: img.ChannelOrder.rgba);

    OpenGLController()
        .openglFFI
        .addSampler2DUniform(uniformName, rgba.width, rgba.height, decodedBytes);
  }


  // where the widget image is stored
  static CapturedWidget captured = CapturedWidget(null, null);

  // capture image retry counter
  static int captureRetryCounter = 0;

  static final Completer<bool> completer = Completer();

  /// Capture widget with the given [widgetKey]
  // TODO: find a better way to capture the widget when the issue will
  // be fixed? https://github.com/flutter/flutter/issues/22308
  static Future<bool> captureWidget(GlobalKey widgetKey) async {
    ui.Image? image;

    try {
      RenderRepaintBoundary? boundary =
      widgetKey.currentContext?.findRenderObject() as RenderRepaintBoundary;

      if (captureRetryCounter > 15) completer.complete(false);

      image = await boundary.toImage();

      captured.byteData =
      await image.toByteData(format: ui.ImageByteFormat.rawRgba);
      captured.size = Size(image.width.toDouble(), image.height.toDouble());

      if (captureRetryCounter > 8) {
        completer.complete(true);
      } else {
        Timer(const Duration(milliseconds: 20), () {
          if (!completer.isCompleted) {
            captureWidget(widgetKey);
          }
        });
        captureRetryCounter++;
      }
    } catch (exception) {
      captureRetryCounter++;
      if (captureRetryCounter > 15) completer.complete(false);
      //Delay is required. See Issue https://github.com/flutter/flutter/issues/22308
      Timer(const Duration(milliseconds: 20), () {
        if (!completer.isCompleted) {
          captureWidget(widgetKey);
        }
      });
    }
    print('WIDGET SIZE: ${captured.size}');
    return completer.future;
  }

}