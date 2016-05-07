#version 330


// info about our material
uniform float     ambient = 0.3;                    // ambient factor
uniform sampler2D textureMap;                       // our texture map

in vec4           V;                                // position of fragment after modelView matrix was applied
in vec3           Nv;                               // normal vector for our fragment (inc view matrix)
in vec2           T;                                // coordinates for this fragment within our texture map

#include "outputs.fs"

void main() {
  // start by getting our color from our texture
  vec4 fragcolor = texture(textureMap, T);  
  if (fragcolor.a < 0.2) {
    discard;
  };

  // Do our outputs without any light calculations
  WorldPosOut = vec4((V.xyz / posScale) + 0.5, 1.0); // our world pos adjusted by view scaled so it fits in 0.0 - 1.0 range
  NormalOut = vec4(Nv, 1.0); // our normal adjusted by view
  AmbientOut = vec4(fragcolor.rgb * ambient, 1.0);
  DiffuseOut = vec4(fragcolor.rgb * (1.0 - ambient), 1.0);
  SpecularOut = vec4(0.0, 0.0, 0.0, 0.0);
}