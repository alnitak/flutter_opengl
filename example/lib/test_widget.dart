import 'package:flutter/material.dart';

import 'shader_widget.dart';
import 'shadertoy.dart';

/// A widget that uses [ShaderWidget] by grabbing it
/// and passing the image to the shader as a texture
class TestWidget extends StatelessWidget {
  /// the ShaderToy code (the last string in the URL)
  final String shaderToyCode;

  const TestWidget({
    Key? key,
    required this.shaderToyCode,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    int shaderToyIndex = shaderToy.indexWhere((element) =>
      element['url']!.contains(shaderToyCode));
    return Padding(
      padding: const EdgeInsets.all(30.0),
      child: ShaderWidget(
        shaderToyIndex: shaderToyIndex,
        child: Container(
          padding: const EdgeInsets.all(22),
          decoration: BoxDecoration(
            color: const Color(0xffececec),
            border: Border.all(width: 2, color: const Color(0xFF2F87EC)),
          ),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              Column(
                children: [
                  Image.asset('assets/dash.png', height: 100),
                  const SizedBox(height: 12),
                  const Text(
                    'Lorem ipsum dolor sit amet, consectetur adipisici elit, '
                    'sed eiusmod tempor incidunt ut labore et dolore magna '
                    'aliqua. Ut enim ad minim veniam, quis nostrud ',
                    style: TextStyle(color: Colors.black),
                  ),
                ],
              ),
              const SizedBox(height: 30),
              Row(
                mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                children: [
                  ElevatedButton(
                    onPressed: () {},
                    child: const Text('Cancel'),
                  ),
                  ElevatedButton(
                    onPressed: () {},
                    child: const Text('OK'),
                  ),
                ],
              )
            ],
          ),
        ),
      ),
    );
  }
}
