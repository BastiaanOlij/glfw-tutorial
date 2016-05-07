#version 330

out vec2 V;

void main() {
  // our triangle primitive
  // 2--------1/5
  // |        /|
  // |      /  |
  // |    /    |
  // |  /      |
  // |/        |
  //0/3--------4

  const vec2 coord[] = vec2[](
    vec2(-1.0,  1.0),
    vec2( 1.0, -1.0),
    vec2(-1.0, -1.0),
    vec2(-1.0,  1.0),
    vec2( 1.0,  1.0),
    vec2( 1.0, -1.0)
  );

  V = coord[gl_VertexID];
  gl_Position = vec4(V, 0.0, 1.0);
}
