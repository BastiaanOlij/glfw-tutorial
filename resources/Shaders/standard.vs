#version 330

layout (location=0) in vec3	positions;
layout (location=1) in vec3	normals;
layout (location=2) in vec2	texcoords;

uniform vec3      eyePos;         // position of our eye
uniform mat4      model;          // our model matrix
uniform mat4      modelView;      // our model-view matrix
uniform mat3      normalMatrix;   // our normal matrix
uniform mat3      normalView;     // our normalView matrix
uniform mat4      mvp;            // our model-view-projection matrix

// these are in world coordinates
out vec3          E;              // normalized vector pointing from eye to V
out vec3          N;              // normal for our fragment with our normal matrix applied

// these are in view
out vec4          V;              // position of fragment after modelView matrix was applied
out vec3          Nv;             // normal for our fragment with our normalView matrix applied
out vec2          T;              // coordinates for this fragment within our texture map

void main(void) {
  // load up our values
  V = vec4(positions, 1.0);
  N = normals;
  T = texcoords;
  
  // our on screen position by applying our model-view-projection matrix
  gl_Position = mvp * V;

  // E is direction of eye
  E = (model * V).xyz;
  E = normalize(E - eyePos);
  
  // V after our model-view matrix is applied
  V = modelView * V;
  
  // N after our normalView matrix is applied
  Nv = normalize(normalView * N);
  
  // N after our normal matrix is applied
  N = normalize(normalMatrix * N);
}
