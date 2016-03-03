#version 330

layout (location=0) in vec3 positions;

uniform vec3 eyePos;        // our eye position
uniform mat4 projection;    // our projection matrix
uniform mat4 view;          // our view matrix
uniform sampler2D bumpMap;  // our height map

out vec4 V;
out vec3 N;
out vec2 T;

float getHeight(vec2 pos) {
  vec4 col = texture(bumpMap, pos / 10000.0);

  return col.r * 1000.0;
}

vec3 calcNormal(vec2 pos) {
  vec3 cN;

  cN.x = getHeight(vec2(pos.s-10.0,pos.t)) - getHeight(vec2(pos.s+10.0,pos.t));
  cN.y = 20.0;
  cN.z = getHeight(vec2(pos.s,pos.t-10.0)) - getHeight(vec2(pos.s,pos.t+10.0));

  return normalize(cN);
}

void main(void) {
  // get our position, we're ignoring our normal and texture coordinates...
  V = vec4(positions, 1.0);

  // our start scale, note that we may vary this depending on distance of ground to camera
  float scale = 1000.0;

  // and scale it up
  V.x = (V.x * scale);
  V.z = (V.z * scale);

  // Use our eyepos defines our center. Use our center size.
  V.x = V.x + (int(eyePos.x / scale) * scale);
  V.z = V.z + (int(eyePos.z / scale) * scale);

  // and get our height from our hmap.
  V.y = getHeight(V.xz);

  // calculate our normal.
  N = calcNormal(V.xz);
  N = (view * vec4(N+eyePos, 1.0)).xyz;

  // and use our coordinates as texture coordinates in our fragment shader
  T = vec2(V.x / 2000.0, V.z / 2000.0);

  // our on screen position by applying our view and projection matrix
  V = view * V;
  gl_Position = projection * V;
}