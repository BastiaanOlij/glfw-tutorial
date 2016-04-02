// our heightmap shader - tesselation version

// finally our fragment shader which hasn't changed much from our previous example

#version 410 core

uniform float       ambient=0.3;    // ambient factor 
uniform vec3        lightPos;       // our sun position (adjusted by view)
uniform vec3        sunCol = vec3(1.0, 1.0, 1.0); // color of our sun
uniform sampler2D   textureMap;     // our texture map
uniform sampler2D   shadowMap;      // our shadow map

in GS_OUT {
  vec2  T;
  vec3  N;
  vec4  V;
  vec4  Vs;
} fs_in;

out vec4 fragcolor; // our output color

// sample our shadow map
float sampleShadowMap(float pZ, vec2 pCoords) {
  float bias = 0.00005;
  float depth = texture(shadowMap, pCoords).x;
  
  if (pZ - bias > depth) {
    return 0.0;
  } else {
    return 1.0;
  };  
}

// check if we're in shadow..
float shadow(vec4 pVs) {
  float factor;
  
  vec3 Proj = pVs.xyz / pVs.w;
  if ((abs(Proj.x)<0.99) && (abs(Proj.y)<0.99) && (abs(Proj.z)<0.99)) {
    // bring it into the range of 0.0 to 1.0 instead of -1.0 to 1.0
    factor = sampleShadowMap(0.5 * Proj.z + 0.5, vec2(0.5 * Proj.x + 0.5, 0.5 * Proj.y + 0.5));
  } else {
    factor = 1.0;
  };

  return factor;
}

void main() {
  // start by getting our color from our texture
  fragcolor = texture(textureMap, fs_in.T);

  // Get the normalized directional vector between our surface position and our light position
  vec3  L = normalize(lightPos - fs_in.V.xyz);

  // We calculate our ambient color
  vec3  ambientColor = fragcolor.rgb * sunCol * ambient;

  // Check our shadow map
  float shadowFactor = shadow(fs_in.Vs);

  // We calculate our diffuse color, we calculate our dot product between our normal and light
  // direction, note that both were adjusted by our view matrix so they should nicely line up
  float NdotL = max(0.0, dot(fs_in.N, L));

  // and calculate our color after lighting is applied
  vec3 diffuseColor = fragcolor.rgb * sunCol * (1.0 - ambient) * NdotL * shadowFactor;

  // and add them all together
  fragcolor = vec4(clamp(ambientColor+diffuseColor, 0.0, 1.0), 1.0);
}