#version 330

// info about our material
uniform sampler2D textureMap;                       // our texture map

in vec2           T;                                // coordinates for this fragment within our texture map
out vec4          fragcolor;                        // our output color

void main() {
  // start by getting our color from our texture
  fragcolor = texture(textureMap, T);  
}