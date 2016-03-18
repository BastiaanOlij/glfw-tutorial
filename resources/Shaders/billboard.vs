#version 330

// need to adjust this, this shader should make the object always point in the direction of the camera

layout (location=0) in vec3 positions;
layout (location=1) in vec3 normals;
layout (location=2) in vec2 texcoords;

uniform mat4      projection;     // our projection matrix
uniform mat4      modelView;      // our model-view matrix
uniform mat3      normalView;     // our normalView matrix

// these are in view
out vec4          V;              // position of fragment after modelView matrix was applied
out vec3          Nv;             // normal for our fragment with our normalView matrix applied
out vec2          T;              // coordinates for this fragment within our texture map

void main(void) {
  // load up our values
  V = vec4(positions, 1.0);
  vec3 N = normals;
  T = texcoords;

  // we reset part of our rotation in our modelView and normalView
  mat4 adjModelView = modelView;
  adjModelView[0][0] = 1.0;
  adjModelView[0][1] = 0.0;
  adjModelView[0][2] = 0.0;
  adjModelView[2][0] = 0.0;
  adjModelView[2][1] = 0.0;
  adjModelView[2][2] = 1.0;

  mat3 adjNormalView = normalView;
  adjNormalView[0][0] = 1.0;
  adjNormalView[0][1] = 0.0;
  adjNormalView[0][2] = 0.0;
  adjNormalView[2][0] = 0.0;
  adjNormalView[2][1] = 0.0;
  adjNormalView[2][2] = 1.0;
  
  // our on screen position by applying our model-view-projection matrix
  gl_Position = projection * adjModelView * V;
  
  // V after our model-view matrix is applied
  V = adjModelView * V;
  
  // N after our normalView matrix is applied
  Nv = normalize(adjNormalView * N);  
}
