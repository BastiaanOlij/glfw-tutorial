#version 330

#ifdef textured
uniform sampler2D textureMap;                       // our texture map
in vec2           T;                                // coordinates for this fragment within our texture map
#endif

out vec4          fragcolor;

void main() {
#ifdef textured
  fragcolor = texture(textureMap, T);
  if (fragcolor.a < 0.2) {
    discard;
  };
#else
  // this does nothing, we're only interested in our Z
  fragcolor = vec4(1.0, 1.0, 1.0, 1.0);
#endif
}