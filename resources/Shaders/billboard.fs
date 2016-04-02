#version 330

// info about our light
uniform vec3      lightPos;                         // position of our light after view matrix was applied
uniform float     ambient = 0.3;                    // ambient factor
uniform vec3      lightcol = vec3(1.0, 1.0, 1.0);   // color of the light of our sun

// info about our material
uniform sampler2D textureMap;                       // our texture map

in vec4           V;                                // position of fragment after modelView matrix was applied
in vec3           Nv;                               // normal vector for our fragment (inc view matrix)
in vec2           T;                                // coordinates for this fragment within our texture map

out vec4          fragcolor;                        // our output color

void main() {
  // start by getting our color from our texture
  fragcolor = texture(textureMap, T);  
  if (fragcolor.a < 0.2) {
    discard;
  };
  
  // Get the normalized directional vector between our surface position and our light position
  vec3  L = normalize(lightPos - V.xyz);
  
  // We calculate our ambient color
  vec3  ambientColor = fragcolor.rgb * lightcol * ambient;
  
  // We calculate our diffuse color, we calculate our dot product between our normal and light
  // direction, note that both were adjusted by our view matrix so they should nicely line up
  float NdotL = max(0.0, dot(Nv, L));
  
  // and calculate our color after lighting is applied
  vec3 diffuseColor = fragcolor.rgb * lightcol * (1.0 - ambient) * NdotL;
  
  // and add them all together
  fragcolor = vec4(clamp(ambientColor+diffuseColor, 0.0, 1.0), fragcolor.a);
}