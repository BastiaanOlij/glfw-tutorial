// our heightmap shader - tesselation version

// finally our fragment shader which hasn't changed much from our previous example

#version 410 core

uniform float       ambient=0.3;    // ambient factor 
uniform vec3        lightPos;       // our sun position (adjusted by view)
uniform vec3        sunCol = vec3(1.0, 1.0, 1.0); // color of our sun
uniform sampler2D   textureMap;     // our texture map

in GS_OUT {
  vec2  T;
  vec3  N;
  vec4  V;
  vec4  Vs[3];
} fs_in;

out vec4 fragcolor; // our output color

#include "shadowmap.fs"

void main() {
  // start by getting our color from our texture
  fragcolor = texture(textureMap, fs_in.T);

  // Get the normalized directional vector between our surface position and our light position
  vec3  L = normalize(lightPos - fs_in.V.xyz);

  // We calculate our ambient color
  vec3  ambientColor = fragcolor.rgb * sunCol * ambient;

  // Check our shadow map
  float shadowFactor = shadow(fs_in.Vs[0],fs_in.Vs[1],fs_in.Vs[2]);
  // vec3 shadowFactor = shadowTst(fs_in.Vs[0],fs_in.Vs[1],fs_in.Vs[2]);

  // We calculate our diffuse color, we calculate our dot product between our normal and light
  // direction, note that both were adjusted by our view matrix so they should nicely line up
  float NdotL = max(0.0, dot(fs_in.N, L));

  // and calculate our color after lighting is applied
  vec3 diffuseColor = fragcolor.rgb * sunCol * (1.0 - ambient) * NdotL * shadowFactor;

  // and add them all together
  fragcolor = vec4(clamp(ambientColor+diffuseColor, 0.0, 1.0), 1.0);
}