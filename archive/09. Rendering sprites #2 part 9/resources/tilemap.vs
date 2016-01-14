#version 330

uniform mat4 mvp;
uniform sampler2D mapdata;

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

	const vec3 vertices[] = vec3[](
		vec3(-0.5,  0.5, 0.9),
		vec3( 0.5, -0.5, 0.9),
		vec3(-0.5, -0.5, 0.9),
		vec3(-0.5,  0.5, 0.9),
		vec3( 0.5,  0.5, 0.9),
		vec3( 0.5, -0.5, 0.9)
	);

	const vec2 texcoord[] = vec2[](
		vec2(         0.0, 31.0 / 256.0),
		vec2(31.0 / 256.0,          0.0),
		vec2(         0.0,          0.0),
		vec2(         0.0, 31.0 / 256.0),
		vec2(31.0 / 256.0, 31.0 / 256.0),
		vec2(31.0 / 256.0,          0.0)
	);

  // now figure out for which tile we are handling our vertex
	int v = gl_VertexID % 6;
	int i = (gl_VertexID - v) / 6;
  // and for which cell
	int x = i % 40;
	int y = (i - x) / 40;

  // figure out our vertex position
	vec4 V = vec4((vertices[v] + vec3(float(x - 20), float(y - 20), 0.0)), 1.0);
  
  // and project it
  gl_Position = mvp * V;

  // now figure out our texture coord
  int ti = int(texture(mapdata, vec2((float(x) + 0.5) / 40.0, (float(y) + 0.5) / 40.0)).r * 256.0);
  int s = ti % 8;
  int t = (ti - s) / 8;
  T = texcoord[v] + vec2((float(s * 32) + 0.5) / 256.0, (float(t * 32) + 0.5) / 256.0);
}