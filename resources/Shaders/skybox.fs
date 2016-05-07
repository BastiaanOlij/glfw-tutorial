#version 330

// info about our material
uniform sampler2D textureMap;                       // our texture map
in vec2           T;                                // coordinates for this fragment within our texture map

#include "outputs.fs"

void main() {
  WorldPosOut = vec4(0.0, 0.0, 0.0, 1.0); // we don't care
  NormalOut = vec4(0.0, 0.0, 1.0, 1.0); // we don't care

  AmbientOut = vec4(texture(textureMap, T).rgb, 1.0);
  DiffuseOut = vec4(0.0, 0.0, 0.0, 0.0);
  SpecularOut = vec4(0.0, 0.0, 0.0, 0.0);    
}