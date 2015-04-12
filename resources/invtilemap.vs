#version 330

uniform mat4 mvp;
uniform mat4 invmvp;

out vec2 T;

void main() {
	// our triangle primitive
	// 2--------1/5
	// |        /|
	// |      /  |
	// |    /    |
	// |  /      |
	// |/        |
	//0/3--------4

	const vec2 vertices[] = vec2[](
		vec2(-1.0, -1.0),
		vec2( 1.0,  1.0),
		vec2(-1.0,  1.0),
		vec2(-1.0, -1.0),
		vec2( 1.0, -1.0),
		vec2( 1.0,  1.0)
	);

  // Get our vertice
	vec4 V = vec4(vertices[gl_VertexID], 0.0, 1.0);
  
  // and project it as is
  gl_Position = V;
  
  // now apply the inverse of our projection and use the x/y 
  T = (invmvp * V).xy;
}