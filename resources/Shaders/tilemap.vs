#version 330

uniform vec2  mapSize;               // number of tiles wide and heigh our tile map is (as float but values should be whole numbers)
uniform int   tilesPerSide;          // number of tiles on each side, we assume square tiles
uniform float textureSize;           // size of the texture, we assume we use square textures

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
		vec3(-0.5,  0.5, 0.0),
		vec3( 0.5, -0.5, 0.0),
		vec3(-0.5, -0.5, 0.0),
		vec3(-0.5,  0.5, 0.0),
		vec3( 0.5,  0.5, 0.0),
		vec3( 0.5, -0.5, 0.0)
	);

	const vec2 texcoord[] = vec2[](
		vec2(0.0, 1.0),
		vec2(1.0, 0.0),
		vec2(0.0, 0.0),
		vec2(0.0, 1.0),
		vec2(1.0, 1.0),
		vec2(1.0, 0.0)
	);

  // now figure out for which tile we are handling our vertex
	int v = gl_VertexID % 6;
	int i = (gl_VertexID - v) / 6;
  // and for which cell
	int x = i % int(mapSize.x);
	int y = (i - x) / int(mapSize.x);

  // figure out our vertex position
	vec4 V = vec4((vertices[v] + vec3(float(x - int(mapSize.x / 2.0)), float(y - int(mapSize.y / 2.0)), 0.0)), 1.0);
  
  // and project it
  gl_Position = mvp * V;

  // now figure out our texture coord
  int ti = int(texture(mapdata, vec2((float(x) + 0.5) / mapSize.x, (float(y) + 0.5) / mapSize.y)).r * 256.0);
  int s = ti % tilesPerSide;
  int t = (ti - s) / tilesPerSide;
  
  float size = (textureSize / float(tilesPerSide));
  vec2 offset = (vec2(float(s), float(t)) * size) + 0.5;
  T = ((texcoord[v] * (size - 1.0)) + offset) / textureSize;
}