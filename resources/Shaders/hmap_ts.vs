// our heightmap shader - tesselation version

// Our vertex shader is alot simpler here as we're basically giving our vertices to our tesselation shader as they are
// obviously we need to scale and center them but thats it.
// We do calculate our screen coordinates because they are handy in our tesselation shader

#version 410 core

layout (location=0) in vec3 positions;

uniform vec3 eyePos;        // our eye position
uniform mat4 projection;    // our projection matrix
uniform mat4 view;          // our view matrix
uniform sampler2D bumpMap;  // our height map
uniform float mapscale = 50000.0; // our map scale
uniform float mapheight = 1000.0; // our map height
uniform float tilescale = 2000.0; // our tile scale

// we only output our projected V
out vec3 Vp;

float getHeight(vec2 pos) {
  vec4 col = texture(bumpMap, pos / mapscale);

  return col.r * mapheight;
}

void main(void) {
  // get our position, we're ignoring our normal and texture coordinates...
  vec4 V = vec4(positions, 1.0);

  // our start scale, note that we may vary this depending on distance of ground to camera
  float scale = tilescale;

  // and scale it up
  V.x = (V.x * scale);
  V.z = (V.z * scale);

  // Use our eyepos defines our center. Use our center size.
  V.x = V.x + (int(eyePos.x / scale) * scale);
  V.z = V.z + (int(eyePos.z / scale) * scale);

  // we use our world position as our output, we don't want the height here yet so our subdivision works properly
  gl_Position = V;

  // now get our height from our hmap because we do need this for our subdivision levels
  V.y = getHeight(V.xz);

  // apply our projection, this will help our tesselation shader
  V = projection * view * V;
  Vp = V.xyz / V.w; // and go to screen coordinates
}