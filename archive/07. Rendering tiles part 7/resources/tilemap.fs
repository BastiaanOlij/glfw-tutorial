#version 330

uniform sampler2D tiles;

in vec2 T;
out vec4 fragcolor;

void main() {
  fragcolor = texture(tiles, T);  
}