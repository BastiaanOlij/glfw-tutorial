// our heightmap shader - tesselation version

// finally our fragment shader which hasn't changed much from our previous example

#version 410 core

uniform float       ambient=0.3;    // ambient factor 
uniform vec3        lightPos;       // our sun position (adjusted by view)
uniform vec3        sunCol = vec3(1.0, 1.0, 1.0); // color of our sun
uniform sampler2D   textureMap;     // our texture map
uniform sampler2D   shadowMap[3];   // our shadow map

in GS_OUT {
  vec2  T;
  vec3  N;
  vec4  V;
  vec4  Vs[3];
} fs_in;

out vec4 fragcolor; // our output color

// Precision ring
//      9 9 9
//      9 1 2
//      9 4 4
const vec2 offsets[] = vec2[](
  vec2( 0.0000,  0.0000),
  vec2( 0.0005,  0.0000),
  vec2( 0.0000,  0.0005),
  vec2( 0.0005,  0.0005),
  vec2(-0.0005,  0.0005),
  vec2(-0.0005,  0.0000),
  vec2(-0.0005, -0.0005),
  vec2( 0.0000, -0.0005),
  vec2(-0.0005, -0.0005)
);

float samplePCF(float pZ, vec2 pCoords, int pMap, int pSamples) {
  float bias = 0.0000005; // our bias
  float result = 1.0; // our result
  float deduct = 0.8 / float(pSamples); // deduct if we're in shadow

  for (int i = 0; i < pSamples; i++) {
    float Depth = texture(shadowMap[pMap], pCoords + offsets[i]).x;
    if (pZ - bias > Depth) {
      result -= deduct;
    };  
  };
    
  return result;
}

// check if we're in shadow..
float shadow(vec4 pVs0, vec4 pVs1, vec4 pVs2) {
  float factor;
  
  vec3 Proj = pVs0.xyz / pVs0.w;
  if ((abs(Proj.x)<0.99) && (abs(Proj.y)<0.99) && (abs(Proj.z)<0.99)) {
    // bring it into the range of 0.0 to 1.0 instead of -1.0 to 1.0
    factor = samplePCF(0.5 * Proj.z + 0.5, vec2(0.5 * Proj.x + 0.5, 0.5 * Proj.y + 0.5), 0, 9);
  } else {
    vec3 Proj = pVs1.xyz / pVs1.w;
    if ((abs(Proj.x)<0.99) && (abs(Proj.y)<0.99) && (abs(Proj.z)<0.99)) {
      // bring it into the range of 0.0 to 1.0 instead of -1.0 to 1.0
      factor = samplePCF(0.5 * Proj.z + 0.5, vec2(0.5 * Proj.x + 0.5, 0.5 * Proj.y + 0.5), 1, 4);
    } else {
      vec3 Proj = pVs2.xyz / pVs2.w;
      if ((abs(Proj.x)<0.99) && (abs(Proj.y)<0.99) && (abs(Proj.z)<0.99)) {
        // bring it into the range of 0.0 to 1.0 instead of -1.0 to 1.0
        factor = samplePCF(0.5 * Proj.z + 0.5, vec2(0.5 * Proj.x + 0.5, 0.5 * Proj.y + 0.5), 2, 4);
      } else {
        factor = 1.0;
      };
    };
  };

  return factor;
}

// Test version that returns color coded shading
vec3 shadowTest(vec4 pVs0, vec4 pVs1, vec4 pVs2) {
  float factor;
  
  vec3 Proj = pVs0.xyz / pVs0.w;
  if ((abs(Proj.x)<0.99) && (abs(Proj.y)<0.99) && (abs(Proj.z)<0.99)) {
    // bring it into the range of 0.0 to 1.0 instead of -1.0 to 1.0
    factor = samplePCF(0.5 * Proj.z + 0.5, vec2(0.5 * Proj.x + 0.5, 0.5 * Proj.y + 0.5), 0, 9);
    return vec3(1.0, factor, factor);
  } else {
    vec3 Proj = pVs1.xyz / pVs1.w;
    if ((abs(Proj.x)<0.99) && (abs(Proj.y)<0.99) && (abs(Proj.z)<0.99)) {
      // bring it into the range of 0.0 to 1.0 instead of -1.0 to 1.0
      factor = samplePCF(0.5 * Proj.z + 0.5, vec2(0.5 * Proj.x + 0.5, 0.5 * Proj.y + 0.5), 1, 4);
      return vec3(factor, 1.0, factor);
    } else {
      vec3 Proj = pVs2.xyz / pVs2.w;
      if ((abs(Proj.x)<0.99) && (abs(Proj.y)<0.99) && (abs(Proj.z)<0.99)) {
        // bring it into the range of 0.0 to 1.0 instead of -1.0 to 1.0
        factor = samplePCF(0.5 * Proj.z + 0.5, vec2(0.5 * Proj.x + 0.5, 0.5 * Proj.y + 0.5), 2, 4);
        return vec3(factor, factor, 1.0);
      } else {
        factor = 1.0;
        return vec3(1.0, 1.0, 1.0);
      };
    };
  };

  //return factor;
}

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