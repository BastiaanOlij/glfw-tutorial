#version 330

uniform sampler2D spriteTexture;

in vec4 V;
in vec2 T;

#include "outputs.fs"

void main() {
  vec4 fragcolor = texture(spriteTexture, T);  
  if (fragcolor.a < 0.5) {
    discard;
  }

  WorldPosOut = vec4((V.xyz / posScale) + 0.5, 1.0);
  NormalOut = vec4(0.0, 0.0, 1.0, 1.0);
  AmbientOut = vec4(fragcolor.rgb * 0.2, 1.0);
  DiffuseOut = vec4(fragcolor.rgb * 0.8, 0.0);
  SpecularOut = vec4(0.0, 0.0, 0.0, 0.0);
}