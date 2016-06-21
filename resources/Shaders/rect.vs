#version 330

uniform mat4      mvp;            // our model-view-projection matrix
out vec2 T;

void main(void) {
  const vec2 coords[] = vec2[](
    vec2(0.0, 1.0),
    vec2(1.0, 0.0),
    vec2(0.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    vec2(1.0, 0.0)
  );

  T = coords[gl_VertexID];
  vec4 V = vec4(T, 0.0, 1.0);
  gl_Position = mvp * V;
}