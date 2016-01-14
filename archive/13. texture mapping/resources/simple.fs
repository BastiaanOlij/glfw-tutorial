#version 330

uniform sampler2D boxtexture;

in vec2 coords;
out vec4 fragcolor;

void main() {
  fragcolor = texture(boxtexture, coords);  
  if (fragcolor.a < 0.5) {
    discard;
  } 
}