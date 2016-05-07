#version 330

uniform sampler2D worldPos;
uniform sampler2D normal;
uniform sampler2D ambient;
uniform sampler2D diffuse;
uniform sampler2D specular;

uniform float posScale = 1000000.0;

// info about our light
uniform vec3      lightPos;                         // position of our light after view matrix was applied
uniform vec3      lightCol = vec3(1.0, 1.0, 1.0);   // color of the light of our sun

#include "shadowmap.fs"

in vec2 V;
out vec4 fragcolor;

#ifdef barreldist
// inspired by https://www.youtube.com/watch?v=B7qrgrrHry0 => http://pastebin.com/1Vanw5Q9
uniform vec2 center = vec2(0.0, 0.0);               // for now center is center but we'll be adjusting this for left/right eye

float distort(float f, float r) {
  float f2 = f * f;
  float f3 = f2 * f;
  float f4 = f3 * f;
  return (0.44*f3 + 0.96*f4*f + r)/(1 + 0.66*f2 + 1.2*f4);
}

vec2 barreldist(vec2 pIn) {
  // Start by adjusting our coordinate to center and slightly scaling it down
  vec2 T = pIn - center;
  T = T / 1.2;
  // T = T * 4.0; 

  float r = sqrt(T.x * T.x + T.y * T.y);
  float f = r;
  // !BAS! which is faster, function or writing it out?
  f = distort(f, r);
  f = distort(f, r);
  f = distort(f, r);
  f = distort(f, r);
  f = distort(f, r);
  f = distort(f, r);
  f = distort(f, r);
  f = distort(f, r);
  f = distort(f, r);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
  f = r/f;
  T = T * f;
  // float2 tc = LensCenter + Scale * theta1;

  // finally adjust to texture coordinates..
  return (T + center + 1.0) / 2.0;
}
#endif

void main() {
  // get our values...
#ifdef barreldist
  vec2 T = barreldist(V);
#else
  vec2 T = (V + 1.0) / 2.0;
#endif
  vec4 ambColor = texture(ambient, T);  
  if (ambColor.a < 0.1) {
    // if no alpha is set, there is nothing here!
    fragcolor = vec4(0.0, 0.0, 0.0, 1.0);
#ifdef barreldist
  } else if ((T.x < 0.0) || (T.x > 1.0) || (T.y < 0.0) || (T.y > 1.0)) {
    fragcolor = vec4(0.0, 0.0, 0.0, 1.0);
#endif
  } else {
    vec4 V = vec4((texture(worldPos, T).xyz - 0.5) * posScale, 1.0);
    vec3 difColor = texture(diffuse, T).rgb;
    vec3 N = texture(normal, T).xyz;
    vec4 specColor = texture(specular, T);

    // we'll add shadows back in a minute
    float shadowFactor = shadow(V);

    // Get the normalized directional vector between our surface position and our light position
    vec3  L = normalize(lightPos - V.xyz);
    float NdotL = max(0.0, dot(N, L));
    difColor = difColor * NdotL * lightCol * shadowFactor;

    float shininess = specColor.a * 256.0;
    if ((NdotL != 0.0) && (shininess != 0.0)) {
      // slightly different way to calculate our specular highlight
      vec3  halfVector  = normalize(L - normalize(V.xyz));
      float nxHalf = max(0.0, dot(N, halfVector));
      float specPower = pow(nxHalf, shininess);
      
      specColor = vec4(lightCol * specColor.rgb * specPower * shadowFactor, 1.0);
    } else {
      specColor = vec4(0.0, 0.0, 0.0, 0.0);
    };

    fragcolor = vec4(ambColor.rgb + difColor + specColor.rgb, 1.0);
  }
}