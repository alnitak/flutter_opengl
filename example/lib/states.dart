import 'package:flutter/material.dart';
import 'package:riverpod/riverpod.dart';

/// FPS got when the renderer is started in controls.dart
final stateFPS = StateProvider<double>((ref) => 0.0);

/// shader URL changed when pressing a button in controls.dart
final stateUrl = StateProvider<String>((ref) => '');

final stateTextureCreated = StateProvider<bool>((ref) => false);
final stateTextureSize = StateProvider<Size>((ref) => const Size(600, 337));
final stateTextureId = StateProvider<int>((ref) => -1);
/// current index in the shaderToy list
final stateShaderIndex = StateProvider<int>((ref) => -1);

/// Absolute video file path
final statePickedVideo = StateProvider<String>((ref) => '');

/// The VideoCapture state
final stateCaptureRunning = StateProvider<bool>((ref) => false);


enum TextureFilter {
  nearest,
  linear,
  mipmap,
}

enum TextureWrap {
  clamp,
  repeat,
}

class TextureParams {
  final String assetImage;
  final TextureFilter filter;
  final TextureWrap wrap;
  final bool vFlip;

  TextureParams({
    this.assetImage = '',
    this.filter = TextureFilter.linear,
    this.wrap = TextureWrap.repeat,
    this.vFlip = false,
  });

  TextureParams copyWith({
    String? assetsImage,
    TextureFilter? filter,
    TextureWrap? wrap,
    bool? vFlip,
  }) {
    return TextureParams(
      assetImage: assetsImage ?? this.assetImage,
      filter: filter ?? this.filter,
      wrap: wrap ?? this.wrap,
      vFlip: vFlip ?? this.vFlip,
    );
  }
}

final stateChannel0 = StateProvider<TextureParams>((ref) => TextureParams());
final stateChannel1 = StateProvider<TextureParams>((ref) => TextureParams());
final stateChannel2 = StateProvider<TextureParams>((ref) => TextureParams());
final stateChannel3 = StateProvider<TextureParams>((ref) => TextureParams());
