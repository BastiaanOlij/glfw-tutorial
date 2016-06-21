#version 330

uniform vec3      matColor;

in float          alp;
out vec4          fragcolor;

void main() {
  fragcolor = vec4(matColor, alp * alp);
}