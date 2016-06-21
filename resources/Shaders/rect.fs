#version 330

uniform sampler2D textureMap;                       // our texture map
in vec2           T;                                // coordinates for this fragment within our texture map

out vec4          fragcolor;

void main() {
#ifdef DEPTHMAP
  float depth = texture(textureMap, T).r;
  if (depth == 1.0) {
    fragcolor = vec4(0.0, 0.0, 0.0, 1.0);
  } else {
    fragcolor = vec4(depth, depth, depth, 1.0);
  }
#else
  fragcolor = vec4(texture(textureMap, T).rgb, 1.0);
#endif
}