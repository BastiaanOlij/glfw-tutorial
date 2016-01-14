#version 330

uniform sampler2D mapdata;
uniform sampler2D tiles;

in vec2 T;
out vec4 fragcolor;

void main() {
  vec2 to, ti;
  
  // our tiles are 100.0 x 100.0 sized, need to map that
  to = T / 100.0;
  ti = vec2(floor(to.x), floor(to.y));
  to = to - ti;
  
  // our bitmaps are 32.0 x 32.0 within a 256x256 bitmap:
  to = 31.0 * to / 256.0;
  
  // now add an offset for our tile
  ti += 20.0;
  int tileidx = int(texture(mapdata, (ti + 0.5) / 40.0).r * 256.0);
  int s = tileidx % 8;
  int t = (tileidx - s) / 8;
  to = to + vec2((float(s * 32) + 0.5) / 256.0, (float(t * 32) + 0.5) / 256.0);
  
  // and get out color
  fragcolor = texture(tiles, to);  
}