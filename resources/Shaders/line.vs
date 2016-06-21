#version 330

layout (location=0) in vec3 positions;
uniform mat4 mvp;

out float alp;

void main() {
  vec4 V = vec4(positions, 1.0);
  gl_Position = mvp * V;

  if (gl_VertexID % 2 == 0) {
    alp = 1.0;
  } else {
    alp = -1.0;
  };
}