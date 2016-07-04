#version 330

#include "inputs.fs"

// info about our light
uniform vec3      lightPos;                         // position of our light after view matrix was applied
uniform vec3      lightCol;                         // color of the light of our sun
uniform sampler2D lightMap;                         // light map
uniform float     radius = 100.0;                   // maximum distance to light at which we still illuminate things
uniform float     attConstant = 0.0;
uniform float     attLinear = 0.0;
uniform float     attExp = 0.0;

#include "shadowmap.fs"
#include "barrel.inc"

in vec2 V;
in float R;

out vec4 fragcolor;

void main() {
  // get our values...
#ifdef barreldist
  vec2 T = barreldist(V, 1);
  if ((T.x < 0.0) || (T.x > 1.0) || (T.y < 0.0) || (T.y > 1.0)) {
    discard;
  } else {
#else
  vec2 T = (V + 1.0) / 2.0;
#endif
    vec4 V = vec4((texture(worldPos, T).xyz - 0.5) * posScale, 1.0);
    vec3 difColor = texture(diffuse, T).rgb;
    vec4 specColor = texture(specular, T);
    vec3 N = (texture(normal, T).xyz - 0.5) * 2.0;
    float shadowFactor = 1.0;
    vec3 lColor = lightCol;

    // we're going to use our shadow maps projection matrix to limit our light
    vec4 Vs = shadowMat[0] * V;
    vec3 Proj = Vs.xyz / Vs.w;
    if ((abs(Proj.x) < 1.00) && (abs(Proj.y) < 1.00) && (abs(Proj.z) < 1.00)) {
      vec2 coords = vec2(0.5 * Proj.x + 0.5, 0.5 * Proj.y + 0.5);
      // bring it into the range of 0.0 to 1.0 instead of -1.0 to 1.0
      shadowFactor = samplePCF(0.5 * Proj.z + 0.5, coords, 0, 9);

      lColor = lColor * texture(lightMap, 1.0-coords).rgb;
    } else {
      // no point in doing this..
      discard;
    };

    // Get the normalized directional vector between our surface position and our light position
    vec3  LmV = lightPos - V.xyz;
    float Dist = length(LmV);
    if (Dist > radius) {
      // we're too far away from the light for it to effect us
      discard;
    };

    // get our light vector
    vec3  L = normalize(LmV);
    float NdotL = max(0.0, dot(N, L));
    difColor = difColor * NdotL * lColor * shadowFactor;

    float shininess = specColor.a * 256.0;
    if ((NdotL != 0.0) && (shininess != 0.0)) {
      // slightly different way to calculate our specular highlight
      vec3  halfVector  = normalize(L - normalize(V.xyz));
      float nxHalf = max(0.0, dot(N, halfVector));
      float specPower = pow(nxHalf, shininess);
      
      specColor = vec4(lColor * specColor.rgb * specPower * shadowFactor, 1.0);
    } else {
      specColor = vec4(0.0, 0.0, 0.0, 0.0);
    };

    // now calculate our attenuation
    float attenuation = attConstant + (attLinear * Dist) + (attExp * Dist * Dist);

    // and calculate our final color
    if (attenuation < 1.0) {
      fragcolor = vec4(difColor + specColor.rgb, 1.0);
    } else {
      fragcolor = vec4((difColor + specColor.rgb) / attenuation, 1.0);
    };
#ifdef barreldist
  }
#endif
}