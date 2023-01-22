List<String> shaders = [
  '''
#version 330 core
layout (location = 0) in vec3 aPos;
void main(){
   gl_Position = vec4(aPos, 1.0);
}
''',
  '''
#version 330 core
uniform float time;
out vec4 color;
void main() {
//  color=vec4(gl_Position.x, gl_Position.y, time/10.);
  color=vec4(1.0, 0.0, 1.0, 1.0);
}
'''
];
