#version 330

uniform mat4 mvp;
layout (location=0) in vec3	vertices;
layout (location=1) in vec3	colors;

out vec4 color;

void main() {
  gl_Position = mvp * vec4(vertices, 1.0);
  color = vec4(colors, 1.0);
}