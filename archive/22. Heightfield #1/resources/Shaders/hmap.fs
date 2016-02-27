#version 330

uniform float       ambient=0.3;    // ambient factor 
uniform vec3        lightPos;       // our sun position (adjusted by view)
uniform vec3        sunCol = vec3(1.0, 1.0, 1.0); // color of our sun
uniform sampler2D   textureMap;     // our texture map

in vec4 V;
in vec3 N;
in vec2 T;
out vec4 fragcolor; // our output color

void main() {
  // start by getting our color from our texture
  fragcolor = texture(textureMap, T);

  // Get the normalized directional vector between our surface position and our light position
  vec3  L = normalize(lightPos - V.xyz);

  // We calculate our ambient color
  vec3  ambientColor = fragcolor.rgb * sunCol * ambient;

  // We calculate our diffuse color, we calculate our dot product between our normal and light
  // direction, note that both were adjusted by our view matrix so they should nicely line up
  float NdotL = max(0.0, dot(N, L));

  // and calculate our color after lighting is applied
  vec3 diffuseColor = fragcolor.rgb * sunCol * (1.0 - ambient) * NdotL;

  // and add them all together
  fragcolor = vec4(clamp(ambientColor+diffuseColor, 0.0, 1.0), 1.0);
}