#version 430 core
// in from cpu
layout(location = 0) in vec2 a_pos;

// out to frag
layout(location = 0) out vec3 o_pos;

void main() {
  gl_Position = vec4(a_pos.x, a_pos.y, 0.0, 1.0);
  o_pos = vec3(a_pos, 0.0);
}