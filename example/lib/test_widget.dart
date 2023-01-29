import 'package:flutter/material.dart';

import 'shader_widget.dart';

/// A widget that uses [ShaderWidget] by grabbing it
/// and passing the image to the shader as a texture
class TestWidget extends StatelessWidget {
  final int shaderToyIndex;

  const TestWidget({
    Key? key,
    required this.shaderToyIndex,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
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
              Row(
                children: [
                  Image.asset('assets/dash.png', height: 100),
                  const SizedBox(width: 12),
                  const Expanded(
                    child: Text(
                      'Lorem ipsum dolor sit amet, consectetur adipisici elit, '
                      'sed eiusmod tempor incidunt ut labore et dolore magna '
                      'aliqua. Ut enim ad minim veniam, quis nostrud '
                      'exercitation ullamco laboris nisi ut aliquid ex ea '
                      'commodi consequat. Quis aute iure reprehenderit in '
                      'voluptate velit esse cillum dolore eu fugiat '
                      'nulla pariatur. Excepteur sint obcaecat cupiditat non '
                      'proident, sunt in culpa qui officia deserunt '
                      'mollit anim id est laborum.',
                      style: TextStyle(color: Colors.black),
                    ),
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
