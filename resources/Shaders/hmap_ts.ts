// our heightmap shader - tesselation version, mostly based on superbible 6

// Our tesselation control shader tell OpenGL how much detail we require for this patch. We thus get a whole quad as input.
// The idea is that you specify the tesselation level for each edge of the quad.

#version 410 core

// we output 4 vertices, we're still doing quads
layout (vertices = 4) out;

const float prec = 50.0; // our precision factor, the higher, the more polies we generate
const float falloff = 1.5; // our low res quads may fall off screen yet after tessellation and adjustment appear visible so we cast a wider net.

uniform int maxTessGenLevel = 16;  // maximum tesselation level

// input from our vertex shader, note that this is an array of 4!!
in vec3 Vp[];

void main(void) {
  if (gl_InvocationID == 0) {
    // get our screen coords
    vec3 V0 = Vp[0];
    vec3 V1 = Vp[1];
    vec3 V2 = Vp[2];
    vec3 V3 = Vp[3];
    
    // check if we're off screen and if so, no tessellation => nothing rendered
    if (
      ((V0.z <= 0.0) && (V1.z <= 0.0) && (V2.z <= 0.0) && (V3.z <= 0.0))              // behind camera
      || ((V0.x <= -falloff) && (V1.x <= -falloff) && (V2.x <= -falloff) && (V3.x <= -falloff)) // to the left
      || ((V0.x >=  falloff) && (V1.x >=  falloff) && (V2.x >=  falloff) && (V3.x >=  falloff)) // to the right
      || ((V0.y <= -falloff) && (V1.y <= -falloff) && (V2.y <= -falloff) && (V3.y <= -falloff))   // to the top
      || ((V0.y >=  falloff) && (V1.y >=  falloff) && (V2.y >=  falloff) && (V3.y >=  falloff)) // to the bottom
    ) {
      gl_TessLevelOuter[0] = 0.0;
      gl_TessLevelOuter[1] = 0.0;
      gl_TessLevelOuter[2] = 0.0; 
      gl_TessLevelOuter[3] = 0.0; 
      gl_TessLevelInner[0] = 0.0;
      gl_TessLevelInner[1] = 0.0;
    } else {
      float level0 = maxTessGenLevel;
      float level1 = maxTessGenLevel;
      float level2 = maxTessGenLevel;
      float level3 = maxTessGenLevel;

      // We look at the lenght of each edge, the longer it is, the more detail we want to add
      // If any edge goes through our Camera plane we set maximum level
      
      if ((V0.z>0.0) && (V2.z>0.0)) {
        level0 = min(maxTessGenLevel, max(length(V0.xy - V2.xy) * prec, 1.0));
      }
      if ((V0.z>0.0) && (V1.z>0.0)) {
        level1 = min(maxTessGenLevel, max(length(V0.xy - V1.xy) * prec, 1.0));
      }
      if ((V1.z>0.0) && (V1.z>0.0)) {
        level2 = min(maxTessGenLevel, max(length(V1.xy - V3.xy) * prec, 1.0));
      }
      if ((V3.z>0.0) && (V2.z>0.0)) {
        level3 = min(maxTessGenLevel, max(length(V3.xy - V2.xy) * prec, 1.0));
      }

      gl_TessLevelOuter[0] = level0;
      gl_TessLevelOuter[1] = level1;
      gl_TessLevelOuter[2] = level2;  
      gl_TessLevelOuter[3] = level3;  
      gl_TessLevelInner[0] = min(level1, level3);
      gl_TessLevelInner[1] = min(level0, level2);
    }
  };
  
  // just copy our vertices as control points
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}