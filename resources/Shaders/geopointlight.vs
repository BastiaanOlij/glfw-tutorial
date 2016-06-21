#version 330

#define PI 3.1415926535897932384626433832795

uniform float   radius = 100.0;
uniform mat4    projection;
uniform vec3    lightPos;

#include "barrel.inc"

out vec2 V;
out float R;

void main() {
  // doing full screen for a second, we're going to optimize this by drawing a circle !!
  // we're going to do a waver
  // first point is in the center
  // then each point is rotated by 10 degrees

  //           4
  //      3   ---   5
  //       /\  |  /\  
  //     /    \|/    \
  //   2|------1------|6
  //     \    /|\    /
  //       \/  |  \/  
  //      9   ---  7
  //           8

  if (gl_VertexID == 0) {
    vec4 Vproj = projection * vec4(lightPos, 1.0);
    V = Vproj.xy / Vproj.w;
    R = radius;
  } else {
    float ang = (gl_VertexID - 1) * 10;
    ang = ang * PI / 180.0;
    vec4 Vproj = projection * vec4(lightPos.x - (radius * cos(ang)), lightPos.y + (radius * sin(ang)), lightPos.z, 1.0);
    V = Vproj.xy / Vproj.w;
    R = 0.0;
  };
#ifdef barreldist
  V = barreldist(V, 0);
#endif
  gl_Position = vec4(V, 0.0, 1.0);
}
