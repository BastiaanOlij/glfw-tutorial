#version 330

// info about our material
uniform float     alpha = 1.0;                      // alpha for our material
uniform float     ambient = 0.3;                    // ambient factor
#ifdef textured
uniform sampler2D textureMap;                       // our texture map
#else
uniform vec3      matColor = vec3(0.8, 0.8, 0.8);   // color of our material
#endif
uniform vec3      matSpecColor = vec3(1.0, 1.0, 1.0); // specular color of our material
uniform float		  shininess = 100.0;                // shininess

#ifdef reflect
uniform sampler2D reflectMap;                       // our reflection map

// these are in world coordinates
in vec3           E;                                // normalized vector pointing from eye to V
in vec3           N;                                // normal vector for our fragment
#endif

// these in view
in vec4           V;                                // position of fragment after modelView matrix was applied
in vec3           Nv;                               // normal vector for our fragment (inc view matrix)
in vec2           T;                                // coordinates for this fragment within our texture map

#include "outputs.fs"

void main() {
#ifdef textured
  // start by getting our color from our texture
  vec4 fragcolor = texture(textureMap, T);  
  fragcolor.a = fragcolor.a * alpha;
  if (fragcolor.a < 0.2) {
    discard;
  };
#else
  // Just set our color
  vec4 fragcolor = vec4(matColor, alpha);
#endif
  
  // Do our outputs without any light calculations
  WorldPosOut = vec4((V.xyz / posScale) + 0.5, 1.0); // our world pos adjusted by view scaled so it fits in 0.0 - 1.0 range
  NormalOut = vec4(Nv, 1.0); // our normal adjusted by view

#ifdef reflect
  // add in our reflection, this is one of the few places where world coordinates are paramount. 
  vec3  r = reflect(E, N);
  vec2  rc = vec2((r.x + 1.0) / 4.0, (r.y + 1.0) / 2.0);
  if (r.z < 0.0) {
   r.x = 1.0 - r.x;
  };

  // Add our reflection into our ambient so we don't apply lighting, we're reflecting light here
  AmbientOut = clamp(vec4((fragcolor.rgb * ambient) + texture(reflectMap, rc).rgb, 1.0), 0.0, 1.0);
#else
  AmbientOut = vec4(fragcolor.rgb * ambient, 1.0);
#endif

  DiffuseOut = vec4(fragcolor.rgb * (1.0 - ambient), 1.0);
  SpecularOut = clamp(vec4(matSpecColor, shininess / 256.0), 0.0, 1.0);
}