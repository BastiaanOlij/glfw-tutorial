#version 330

uniform sampler2D textureMap;                       // our texture map

in vec2           T;                                // coordinates for this fragment within our texture map
out vec4          fragcolor;

void main() {
  fragcolor = texture(textureMap, T);
  if (fragcolor.a < 0.2) {
    discard;
  };
}