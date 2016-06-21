#ifdef barreldist
// inspired by https://www.youtube.com/watch?v=B7qrgrrHry0 => http://pastebin.com/1Vanw5Q9
uniform vec2 center = vec2(0.0, 0.0);               // for now center is center but we'll be adjusting this for left/right eye

float distort(float f, float r) {
  float f2 = f * f;
  float f3 = f2 * f;
  float f4 = f3 * f;
  return (0.44*f3 + 0.96*f4*f + r)/(1 + 0.66*f2 + 1.2*f4);
}

vec2 barreldist(vec2 pIn, int pForFS) {
  // Start by adjusting our coordinate to center and slightly scaling it down
  vec2 T = pIn - center;
  if (pForFS != 0) {
    T = T / 1.2;
  };

  float r = sqrt(T.x * T.x + T.y * T.y);
  float f = r;
  // !BAS! which is faster, function or writing it out?
  f = distort(f, r);
  f = distort(f, r);
  f = distort(f, r);
  f = distort(f, r);
  f = distort(f, r);
  f = distort(f, r);
  f = distort(f, r);
  f = distort(f, r);
  f = distort(f, r);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
//  f = (0.44*f*f*f + 0.96*f*f*f*f*f + r)/(1+0.66*f*f+1.2*f*f*f*f);
  if (pForFS == 0) {
    f = f/r;
    T = T * f;
    T = T * 1.2;
    return T + center;
  } else {
    f = r/f;
    T = T * f;
    // float2 tc = LensCenter + Scale * theta1;

    // finally adjust to texture coordinates..
    return (T + center + 1.0) / 2.0;
  };
}
#endif