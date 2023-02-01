import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_opengl/flutter_opengl.dart';

/// Page to edit and compile the shader
///
/// In this example there is button to add a vec3 "TEST" uniform
/// and other 2 button to increase/decrease its x value.
/// To test this:
/// - add into the fragment shader the new uniform "uniform vec3 TEST;"
/// - use "TEST.x" somewhere in the code
/// - press the "compile shader" button
/// - press the "add TEST" button
/// - try the behavior by pressing the increment/decrement TEST buttons
class EditShader extends StatelessWidget {
  const EditShader({
    Key? key,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    double test = 0.1;

    ValueNotifier<String> compileError = ValueNotifier('');
    String vs = OpenGLController().openglFFI.getVertexShader();
    String fs = OpenGLController().openglFFI.getFragmentShader();

    TextEditingController vsController = TextEditingController(text: vs);
    TextEditingController fsController = TextEditingController(text: fs);
    InputDecoration inputDecoration = const InputDecoration(
      contentPadding: EdgeInsets.all(8),
      border: OutlineInputBorder(),
      filled: true,
      fillColor: Colors.black,
    );

    return Column(
      children: [
        Wrap(
          runSpacing: 4,
          spacing: 4,
          children: [
            const SizedBox(width: 12),

            /// Compile button
            ElevatedButton(
              style: const ButtonStyle(
                  backgroundColor: MaterialStatePropertyAll(Colors.green)),
              onPressed: () {
                String err = OpenGLController().openglFFI.setShader(
                      true,
                      vsController.text,
                      fsController.text,
                    );
                compileError.value = err;
                if (err.isNotEmpty) {
                  ScaffoldMessenger.of(context).showSnackBar(const SnackBar(
                    content: Text('Error compiling shader'),
                    duration: Duration(seconds: 2),
                  ));
                } else {
                  // When compiling a new shader, the ShaderToy uniforms
                  // are not set by default like [setShaderToy()] does.
                  // So add them programmatically
                  OpenGLController().openglFFI.addShaderToyUniforms();
                }
              },
              child: const Text('compile shader'),
            ),

            /// TEST
            ElevatedButton(
              onPressed: () {
                test = 0.0;
                OpenGLController().openglFFI.addVec3Uniform(
                  'TEST',
                  [test, 0.2, 0.3],
                );
              },
              child: const Text('add "TEST" vec3'),
            ),
            ElevatedButton(
              onPressed: () {
                OpenGLController().openglFFI.setVec3Uniform(
                  'TEST',
                  [test, 0.2, 0.3],
                );
                test += 0.1;
              },
              child: const Text('"TEST.x +=0.1"'),
            ),
            ElevatedButton(
              onPressed: () {
                OpenGLController().openglFFI.setVec3Uniform(
                  'TEST',
                  [test, 0.2, 0.3],
                );
                test -= 0.1;
              },
              child: const Text('"TEST.x -=0.1"'),
            ),
          ],
        ),
        const SizedBox(height: 12),

        /// compile error
        ValueListenableBuilder(
          valueListenable: compileError,
          builder: (_, err, __) {
            if (err.isEmpty) return const SizedBox.shrink();
            return Padding(
              padding: const EdgeInsets.only(bottom: 12.0),
              child: ColoredBox(
                color: Colors.red,
                child: Padding(
                  padding: const EdgeInsets.all(8.0),
                  child: Text(
                    err,
                    style: const TextStyle(
                      color: Colors.white,
                      fontWeight: FontWeight.bold,
                    ),
                  ),
                ),
              ),
            );
          },
        ),

        /// Vertex and fragment sources
        Expanded(
          child: SingleChildScrollView(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                const Text(
                  'Vertex shader',
                  textScaleFactor: 1.5,
                  style: TextStyle(fontWeight: FontWeight.bold),
                ),
                TextField(
                  controller: vsController,
                  decoration: inputDecoration,
                  expands: false,
                  minLines: 1,
                  maxLines: null,
                  style:
                      const TextStyle(fontFamily: "JetBrainsMono-Regular.ttf"),
                ),
                const SizedBox(height: 12),
                const Text(
                  'Fragment shader',
                  textScaleFactor: 1.5,
                  style: TextStyle(fontWeight: FontWeight.bold),
                ),
                TextField(
                  controller: fsController,
                  decoration: inputDecoration,
                  expands: false,
                  minLines: 1,
                  maxLines: null,
                  style:
                      const TextStyle(fontFamily: "JetBrainsMono-Regular.ttf"),
                ),
              ],
            ),
          ),
        ),
      ],
    );
  }
}
