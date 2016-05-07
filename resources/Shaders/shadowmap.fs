// functions we include into fragment shaders for our shadow map logic

uniform sampler2D   shadowMap[3];   // our shadow map
uniform mat4        shadowMat[3];   // our shadows view-projection matrix with inverse of our camera view applied
// in vec4             Vs[3];          // our shadow map coordinates

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
float shadow(vec4 pV) {
  float factor;
  vec4  V;
  vec3  Proj;
  
  V = shadowMat[0] * pV;
  Proj = V.xyz / V.w;
  if ((abs(Proj.x) < 0.99) && (abs(Proj.y) < 0.99) && (abs(Proj.z) < 0.99)) {
    // bring it into the range of 0.0 to 1.0 instead of -1.0 to 1.0
    factor = samplePCF(0.5 * Proj.z + 0.5, vec2(0.5 * Proj.x + 0.5, 0.5 * Proj.y + 0.5), 0, 9);
  } else {
    V = shadowMat[1] * pV;
    Proj = V.xyz / V.w;
    if ((abs(Proj.x) < 0.99) && (abs(Proj.y) < 0.99) && (abs(Proj.z) < 0.99)) {
      // bring it into the range of 0.0 to 1.0 instead of -1.0 to 1.0
      factor = samplePCF(0.5 * Proj.z + 0.5, vec2(0.5 * Proj.x + 0.5, 0.5 * Proj.y + 0.5), 1, 4);
    } else {
      V = shadowMat[2] * pV;
      Proj = V.xyz / V.w;
      if ((abs(Proj.x) < 0.99) && (abs(Proj.y) < 0.99) && (abs(Proj.z) < 0.99)) {
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
vec3 shadowTest(vec4 pV) {
  float factor;
  vec4  V;
  vec3  Proj;
  
  V = shadowMat[0] * pV;
  Proj = V.xyz / V.w;
  if ((abs(Proj.x) < 0.99) && (abs(Proj.y) < 0.99) && (abs(Proj.z) < 0.99)) {
    // bring it into the range of 0.0 to 1.0 instead of -1.0 to 1.0
    factor = samplePCF(0.5 * Proj.z + 0.5, vec2(0.5 * Proj.x + 0.5, 0.5 * Proj.y + 0.5), 0, 9);
    return vec3(1.0, factor, factor);
  } else {
    V = shadowMat[1] * pV;
    Proj = V.xyz / V.w;
    if ((abs(Proj.x) < 0.99) && (abs(Proj.y) < 0.99) && (abs(Proj.z) < 0.99)) {
      // bring it into the range of 0.0 to 1.0 instead of -1.0 to 1.0
      factor = samplePCF(0.5 * Proj.z + 0.5, vec2(0.5 * Proj.x + 0.5, 0.5 * Proj.y + 0.5), 1, 4);
      return vec3(factor, 1.0, factor);
    } else {
      V = shadowMat[2] * pV;
      Proj = V.xyz / V.w;
      if ((abs(Proj.x) < 0.99) && (abs(Proj.y) < 0.99) && (abs(Proj.z) < 0.99)) {
        // bring it into the range of 0.0 to 1.0 instead of -1.0 to 1.0
        factor = samplePCF(0.5 * Proj.z + 0.5, vec2(0.5 * Proj.x + 0.5, 0.5 * Proj.y + 0.5), 2, 4);
        return vec3(factor, factor, 1.0);
      } else {
        factor = 1.0;
        return vec3(1.0, 1.0, 1.0);
      };
    };
  };
}