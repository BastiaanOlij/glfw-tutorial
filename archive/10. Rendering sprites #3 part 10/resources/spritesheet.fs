#version 330

uniform sampler2D sprite;

in vec2 T;
out vec4 fragcolor;

void main() {
  fragcolor = texture(sprite, T);  
  if (fragcolor.a < 0.5) {
    discard;
  }
}