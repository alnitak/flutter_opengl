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
  /// Used in the recursive _capture() function to know when must return
  static final Completer<bool> _completer = Completer();

  /// where the widget image is stored
  static final CapturedWidget captured = CapturedWidget(ByteData(0), Size.zero);

  static Future<CapturedWidget> captureWidget(GlobalKey widgetKey) async {
    await _capture(widgetKey, 15);
    return captured;
  }

  /// Capture the widget with the given [widgetKey]
  // TODO: find a better way to capture the widget when the issue will
  // be fixed? https://github.com/flutter/flutter/issues/22308
  static Future<bool> _capture(GlobalKey widgetKey, int retryCounter) async {
    ui.Image? image;

    try {
      RenderRepaintBoundary? boundary =
          widgetKey.currentContext?.findRenderObject() as RenderRepaintBoundary;

      if (retryCounter < 1) _completer.complete(false);

      image = await boundary.toImage();

      captured.byteData =
          await image.toByteData(format: ui.ImageByteFormat.rawRgba) ??
              ByteData(0);
      captured.size = Size(image.width.toDouble(), image.height.toDouble());

      if (retryCounter < 8) {
        _completer.complete(true);
      } else {
        Timer(const Duration(milliseconds: 20), () {
          if (!_completer.isCompleted) {
            _capture(widgetKey, retryCounter);
          }
        });
        retryCounter--;
      }
    } catch (exception) {
      retryCounter--;
      if (retryCounter < 1) _completer.complete(false);
      //Delay is required. See Issue https://github.com/flutter/flutter/issues/22308
      Timer(const Duration(milliseconds: 20), () {
        if (!_completer.isCompleted) {
          _capture(widgetKey, retryCounter);
        }
      });
    }
    return _completer.future;
  }

  /// Load an asset image, flip vertically and
  /// add it to the shader uniform with the name [uniformName]
  static setAssetTexture(String uniformName, String assetName) async {
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

    OpenGLController().openglFFI.addSampler2DUniform(
        uniformName, rgba.width, rgba.height, decodedBytes);
  }
}
