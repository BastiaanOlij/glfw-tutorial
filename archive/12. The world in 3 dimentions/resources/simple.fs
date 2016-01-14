#version 330

in vec3 color;
out vec4 fragcolor;

void main() {
  fragcolor = vec4(color, 1.0);  
}