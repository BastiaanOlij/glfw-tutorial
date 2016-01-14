#version 330

layout (location=0) in vec3	positions;
layout (location=1) in vec3	normals;
layout (location=2) in vec2	texcoords;

uniform mat4      modelView;      // our model-view matrix
uniform mat3      normalMatrix;   // our normal matrix
uniform mat4      mvp;            // our model-view-projection matrix

out vec4          V;              // position of fragment after modelView matrix was applied
out vec3          N;              // normal for our fragment with our normal matrix applied
out vec2          T;              // coordinates for this fragment within our texture map

void main(void) {
  // load up our values
  V = vec4(positions, 1.0);
  N = normals;
  T = texcoords;
  
  // our on screen position by applying our model-view-projection matrix
  gl_Position = mvp * V;
  
  // V after our model-view matrix is applied
  V = modelView * V;
  
  // N after our normal matrix is applied, note that this is adjusted to our view
  N = normalize(normalMatrix * N);
}
