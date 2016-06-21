#version 330

uniform mat4 mvp;              // our model-view-projection matrix
uniform mat4 modelView;				 // our model-view matrix
uniform vec2 textureSize;      // size of our texture in pixels
uniform vec2 spriteLeftTop;    // top/left position of our sprite in our texture
uniform vec2 spriteSize;       // size of our sprite within our texture in pixels

out vec4 V;
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

  // figure out our vertex position
	V = vec4(vertices[gl_VertexID].x * spriteSize.x, vertices[gl_VertexID].y * spriteSize.y, vertices[gl_VertexID].z, 1.0);
  
  // and project it
  gl_Position = mvp * V;
  V = modelView * V;

  // now figure out our texture coord
  T = (spriteLeftTop + (texcoord[gl_VertexID] * spriteSize)) / textureSize;
}