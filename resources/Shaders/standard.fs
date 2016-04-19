#version 330

// info about our light
uniform vec3      lightPos;                         // position of our light after view matrix was applied
uniform float     ambient = 0.3;		                // ambient factor
uniform vec3		  lightcol = vec3(1.0, 1.0, 1.0);	  // color of the light of our sun

// info about our material
uniform float     alpha = 1.0;                      // alpha for our material
#ifdef textured
uniform sampler2D textureMap;                       // our texture map
#else
uniform vec3      matColor = vec3(0.8, 0.8, 0.8);   // color of our material
#endif
uniform vec3      matSpecColor = vec3(1.0, 1.0, 1.0); // specular color of our material
uniform float		  shininess = 100.0;                // shininess

#ifdef reflect
uniform sampler2D reflectMap;                       // our reflection map
#endif

// these are in world coordinates
in vec3           E;                                // normalized vector pointing from eye to V
in vec3           N;                                // normal vector for our fragment

// these in view
in vec4           V;                                // position of fragment after modelView matrix was applied
in vec3           Nv;                               // normal vector for our fragment (inc view matrix)
in vec2           T;                                // coordinates for this fragment within our texture map
in vec4           Vs[3];                            // our shadow map coordinates
out vec4          fragcolor;                        // our output color

#include "shadowmap.fs"

void main() {
#ifdef textured
  // start by getting our color from our texture
  fragcolor = texture(textureMap, T);  
  fragcolor.a = fragcolor.a * alpha;
  if (fragcolor.a < 0.2) {
    discard;
  };
#else
  // Just set our color
  fragcolor = vec4(matColor, alpha);
#endif

  // Get the normalized directional vector between our surface position and our light position
  vec3	L = normalize(lightPos - V.xyz);
  
  // We calculate our ambient color
  vec3  ambientColor = fragcolor.rgb * lightcol * ambient;

  // Check our shadow map
  float shadowFactor = shadow(Vs[0], Vs[1], Vs[2]);
  
  // We calculate our diffuse color, we calculate our dot product between our normal and light
  // direction, note that both were adjusted by our view matrix so they should nicely line up
  float NdotL = max(0.0, dot(Nv, L));
  
  // and calculate our color after lighting is applied
  vec3 diffuseColor = fragcolor.rgb * lightcol * (1.0 - ambient) * NdotL * shadowFactor;

  // now for our specular lighting
	vec3 specColor = vec3(0.0);
  if ((NdotL != 0.0) && (shininess != 0.0)) {
    // slightly different way to calculate our specular highlight
		vec3	halfVector	= normalize(L - normalize(V.xyz));
		float	nxHalf = max(0.0, dot(Nv, halfVector));
		float	specPower = pow(nxHalf, shininess);
		
    specColor = lightcol * matSpecColor * specPower * shadowFactor;
  };

#ifdef reflect
  // add in our reflection, this is one of the few places where world coordinates are paramount. 
  vec3  r = reflect(E, N);
  vec2  rc = vec2((r.x + 1.0) / 4.0, (r.y + 1.0) / 2.0);
  if (r.z < 0.0) {
   r.x = 1.0 - r.x;
  };
  vec3  reflColor = texture(reflectMap, rc).rgb;

  // and add them all together
  fragcolor = vec4(clamp(ambientColor+diffuseColor+specColor+reflColor, 0.0, 1.0), fragcolor.a);
#else
  // and add them all together
  fragcolor = vec4(clamp(ambientColor+diffuseColor+specColor, 0.0, 1.0), fragcolor.a);
#endif
}