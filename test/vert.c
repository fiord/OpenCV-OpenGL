#version 130

// attribute vec3 vertex_position;
// attribute vec4 vertex_color;
uniform mat4 matrix;
varying vec4 out_color;

void main(void) {
  gl_Position = gl_Vertex;
  out_color = gl_Color;;
}
