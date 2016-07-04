#version 330

#include "inputs.fs"

// info about our light
uniform vec3      lightPos;                         // position of our light after view matrix was applied
uniform vec3      lightCol;                         // color of the light of our sun

#include "shadowmap.fs"
#include "barrel.inc"

in vec2 V;
out vec4 fragcolor;

void main() {
  // get our values...
#ifdef barreldist
  vec2 T = barreldist(V, 1);
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
    vec3 N = (texture(normal, T).xyz - 0.5) * 2.0;
    vec4 specColor = texture(specular, T);

    // calculate our shadow factor
    float shadowFactor = cascadedShadow(V);

    // Get the normalized directional vector based on our light position
    vec3  L = normalize(lightPos);
    float NdotL = max(0.0, dot(N, L));
    difColor = difColor * NdotL * lightCol * shadowFactor;

    float shininess = specColor.a * 256.0;
    if ((NdotL > 0.0) && (shininess != 0.0)) {
      vec3 lightReflect = normalize(reflect(L, N));
      float specPower = dot(normalize(V.xyz), lightReflect);
      if (specPower > 0.0) {
        specPower = pow(specPower, shininess);
        specColor = vec4(lightCol * specColor.rgb * specPower * shadowFactor, 1.0);        
      } else {
        specColor = vec4(0.0, 0.0, 0.0, 0.0);
      };
    } else {
      specColor = vec4(0.0, 0.0, 0.0, 0.0);
    };

    fragcolor = vec4(ambColor.rgb + difColor + specColor.rgb, 1.0);
  }
}