import 'package:flutter/material.dart';
import 'package:riverpod/riverpod.dart';

final stateFPS = StateProvider<String>((ref) => '');
final stateUrl = StateProvider<String>((ref) => '');
final stateTextureCreated = StateProvider<bool>((ref) => false);
final stateTextureSize = StateProvider<Size>((ref) => const Size(600, 337));
final stateTextureId = StateProvider<int>((ref) => -1);
final stateShaderButtonId = StateProvider<int>((ref) => -1);
final stateShaderTextureButtonId = StateProvider<int>((ref) => -1);
