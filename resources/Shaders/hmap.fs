#version 330

uniform float       ambient=0.3;    // ambient factor 
uniform sampler2D   textureMap;     // our texture map

in vec4 V;
in vec3 N;
in vec2 T;

#include "outputs.fs"

void main() {
  // start by getting our color from our texture
  vec4 fragcolor = texture(textureMap, T);

  // Do our outputs without any light calculations
  WorldPosOut = vec4((V.xyz / posScale) + 0.5, 1.0); // our world pos adjusted by view scaled so it fits in 0.0 - 1.0 range
  NormalOut = vec4(N, 1.0); // our normal adjusted by view
  AmbientOut = vec4(fragcolor.rgb * ambient, 1.0);
  DiffuseOut = vec4(fragcolor.rgb * (1.0 - ambient), 1.0);
  SpecularOut = vec4(0.0, 0.0, 0.0, 0.0);
}