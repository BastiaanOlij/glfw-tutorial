#version 330

uniform sampler2D spriteTexture;

in vec2 T;
out vec4 fragcolor;

void main() {
  fragcolor = texture(spriteTexture, T);  
  if (fragcolor.a < 0.5) {
    discard;
  }
}