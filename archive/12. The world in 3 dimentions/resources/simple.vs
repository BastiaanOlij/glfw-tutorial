#version 330

layout (location=0) in vec3	positions;
layout (location=1) in vec3	colors;

uniform mat4 mvp;              // our model-view-projection matrix

out vec3 color;

void main(void) {
  // load up our values
	vec4 V = vec4(positions, 1.0);
  color = colors;
  
	// projection of V
	gl_Position = mvp * V;
}
