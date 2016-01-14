/********************************************************
 * Engine is our container for our main program logic
 * 
 * This allows us to only place code in main.c that
 * interacts with the user through our framework and
 * thus change framework if needed.
 * 
 ********************************************************/

#include "engine.h"

//////////////////////////////////////////////////////////
// For now we just some global state to make life easy

EngineError engineErrCallback = NULL;
EngineKeyPressed engineKeyPressedCallback = NULL;

// shader program and buffers
newtileshader(ts);
newspritesheet(sp);

GLuint VAO = 0;
GLuint textures[TEXT_COUNT] = { 0, 0, 0 };

// and some globals for our fonts
FONScontext *	fs = NULL;
int font = FONS_INVALID;
float lineHeight = 0.0f;

// our view matrix
mat4  view;

// and some runtime variables.
double frames = 0.0f;
double fps = 0.0f;
double lastframes = 0.0f;
double lastsecs = 0.0f;

// map data for our background
unsigned char mapdata[1600] = {
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,13,14,15,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,45,13,14,15,29,30,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,13,14,15,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,30,21,22,23,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29, 0, 1, 2,29,29,30,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,24,25,25,25,25,25,25,25,25,25,25,25,26, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,32,33,33,33,33,33,33,33,33,33,33,33,34, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,39,29,29,29,29,32,33,35,41,36,33,33,33,33,33,33,33,34, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,32,33,34,29,32,33,33,33,33,33,33,33,34, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,37,29,29,32,33,34,29,32,33,33,33,33,33,33,33,34, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,47,37,29,29,29,29,32,33,43,25,44,33,33,33,33,33,33,33,34, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,39,29,29,29,39,29,29,29,32,33,33,33,33,33,33,33,35,41,36,33,34, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  39,29,29,29,29,39,37,29,29,37,29,32,33,33,33,33,33,33,33,43,25,44,33,34, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,38,29,29,29,37,29,39,29,29,29,32,33,33,33,33,33,33,33,33,33,33,33,34, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,38,29,29,29,29,29,29,29,40,41,41,41,41,41,41,41,41,41,41,41,42, 8, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
   6, 6, 7, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,28, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  14,14,15, 8, 9, 9, 9, 9, 9, 9, 9, 9,10, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,10,29,29,29,29,29,29,29,29,29,29,29,29,29,
  22,22,23,16,17,17,17,17,17,17,17,17,18,16,17,17,17,17,17,17,17,17,17,17,17,17,18,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,38,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,31,30,
  29,29,29,29,38,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,31,
  38,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,24,25,25,25,25,25,25,25,25,25,25,25,26,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,45,29,32,33,33,33,33,33,33,33,33,35,41,36,34,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,32,33,33,33,33,33,33,33,33,34,47,32,34,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,46,32,33,33,33,33,33,33,33,33,34,47,32,34,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,38,29,29,29,32,33,33,33,33,33,33,33,33,34,47,32,34,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,32,33,33,33,33,33,33,33,33,43,25,44,34,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,46,29,32,33,33,33,33,33,33,33,33,33,33,33,34,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,32,33,33,33,33,33,33,33,33,33,33,33,34,47,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,32,33,33,33,33,33,33,33,33,33,33,33,34,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,40,41,41,41,41,41,41,41,41,41,41,41,42,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,31,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,31,29,29,29,30,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,31,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
  29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29
};

// sprite sheet data for conrad
sprite conradSpriteSheet[] = {
// row 1
     0.0,    0.0, 20.0, 78.0,   2.0, -39.0,  //  0 -- turn from looking left
    20.0,    0.0, 26.0, 78.0,   2.0, -39.0,  //  1
    46.0,    0.0, 26.0, 78.0,   2.0, -39.0,  //  2
    71.0,    0.0, 26.0, 78.0,   2.0, -39.0,  //  3
    96.0,    0.0, 27.0, 78.0,   0.0, -39.0,  //  4
   123.0,    0.0, 26.0, 78.0,   0.0, -39.0,  //  5
   148.0,    0.0, 22.0, 78.0,  -1.0, -39.0,  //  6 -- to looking right
   
   169.0,    0.0, 28.0, 78.0,  -3.0, -39.0,  //  7 -- stop walking right from 15
   198.0,    0.0, 26.0, 78.0,  -3.0, -39.0,  //  8
   224.0,    0.0, 22.0, 78.0,  -1.0, -39.0,  //  9 
   246.0,    0.0, 18.0, 78.0,  -1.0, -39.0,  // 10 -- finish stop walking right
  
   264.0,    0.0, 26.0, 78.0,   1.0, -39.0,  // 11 -- walking right (left foot forward)
   292.0,    0.0, 28.0, 78.0,   8.0, -39.0,  // 12 
   324.0,    0.0, 42.0, 78.0,  15.0, -39.0,  // 13 
   368.0,    0.0, 46.0, 78.0,  17.0, -39.0,  // 14
   413.0,    0.0, 40.0, 80.0,  21.0, -40.0,  // 15 ---- +30.0
   452.0,    0.0, 30.0, 80.0,  -1.0, -40.0,  // 16 -- (right foot forward) 
   482.0,    0.0, 26.0, 80.0,   7.0, -40.0,  // 17
   508.0,    0.0, 38.0, 80.0,  15.0, -40.0,  // 18
   546.0,    0.0, 48.0, 80.0,  20.0, -40.0,  // 19
   594.0,    0.0, 46.0, 80.0,  19.0, -40.0,  // 20
   640.0,    0.0, 36.0, 82.0,  24.0, -41.0,  // 21
   676.0,    0.0, 30.0, 82.0,  29.0, -41.0,  // 22 -- finished walking right +32.0
   
   706.0,    0.0, 28.0, 82.0,   0.0, -41.0,  // 23 (same as 11?)
    
// row 2  
     0.0,   96.0, 20.0, 78.0,   0.0, -39.0,  // 24 -- get down (facing left)
    20.0,   96.0, 22.0, 78.0,  -4.0, -39.0,  // 25
    42.0,   96.0, 26.0, 78.0,  -2.0, -39.0,  // 26
    68.0,   96.0, 28.0, 78.0,  -1.0, -39.0,  // 27
    96.0,   96.0, 30.0, 78.0,   0.0, -39.0,  // 28
   126.0,   96.0, 30.0, 78.0,   0.0, -39.0,  // 29
   156.0,   96.0, 30.0, 78.0,   0.0, -39.0,  // 30
   186.0,   96.0, 30.0, 78.0,   0.0, -39.0,  // 31 -- finish get down + 0.0
   
   216.0,   96.0, 70.0, 78.0, -22.0, -39.0,  // 32 -- start roll to the left
   286.0,   96.0, 80.0, 78.0, -30.0, -39.0,  // 33
   366.0,   96.0, 70.0, 80.0, -32.0, -40.0,  // 34
   436.0,   96.0, 60.0, 80.0, -46.0, -40.0,  // 35
   496.0,   96.0, 58.0, 80.0, -56.0, -40.0,  // 36
   554.0,   96.0, 50.0, 80.0, -68.0, -40.0,  // 37
   604.0,   96.0, 42.0, 80.0, -72.0, -40.0,  // 38
   646.0,   96.0, 42.0, 80.0, -78.0, -40.0,  // 39
   688.0,   96.0, 44.0, 80.0, -84.0, -40.0,  // 40
   732.0,   96.0, 44.0, 80.0, -84.0, -40.0,  // 41 --

// row 3  
     0.0,  188.0, 40.0, 76.0, -90.0, -38.0,  // 42
    40.0,  188.0, 48.0, 76.0, -96.0, -38.0,  // 43
    88.0,  188.0, 48.0, 78.0,-102.0, -39.0,  // 44
   136.0,  188.0, 48.0, 80.0,-108.0, -40.0,  // 45 -- finish roll -108.0
   184.0,  188.0, 46.0, 80.0,   2.0, -40.0,  // 46 -- intermediate get up (facing left) -20
   230.0,  188.0, 36.0, 80.0,   6.0, -40.0,  // 47 -- get up (facing left)
   266.0,  188.0, 32.0, 80.0,   2.0, -40.0,  // 48
   298.0,  188.0, 28.0, 80.0,   0.0, -40.0,  // 49 -- finish get up -20.0
   326.0,  188.0, 26.0, 80.0,   0.0, -40.0,  // 50 -- jump up
   352.0,  188.0, 22.0, 80.0,   2.0, -40.0,  // 51
   373.0,  188.0, 22.0, 80.0,   3.0, -40.0,  // 52
   395.0,  188.0, 26.0, 80.0,   3.0, -40.0,  // 53
   420.0,  188.0, 28.0, 80.0,   3.0, -40.0,  // 54
   448.0,  188.0, 32.0, 80.0,  -1.0, -40.0,  // 55
   480.0,  188.0, 32.0, 80.0,   1.0, -40.0,  // 56
   512.0,  188.0, 28.0, 80.0,  -1.0, -40.0,  // 57
   540.0,  188.0, 30.0, 80.0,  -4.0, -40.0,  // 58
   570.0,  188.0, 32.0, 80.0,  -1.0, -40.0,  // 59
   602.0,  188.0, 34.0, 80.0,   0.0, -40.0,  // 60
   636.0,  188.0, 32.0, 80.0,   1.0, -40.0,  // 61
   668.0,  188.0, 30.0, 80.0,   0.0, -40.0,  // 62
   697.0,  188.0, 32.0, 80.0,   0.0, -40.0,  // 63
   729.0,  188.0, 32.0, 80.0,  -1.0, -40.0,  // 64
   760.0,  182.0, 28.0, 86.0,   0.0, -43.0,  // 65 
   
// row 4   -- 364 - 90
     0.0,  274.0, 30.0, 90.0,   0.0, -46.0,  // 66 -- finish jump up
    30.0,  274.0, 36.0, 90.0,   0.0, -47.0,  // 67 -- not sure about this one..
    66.0,  274.0, 30.0, 92.0,  -3.0, -47.0,  // 68 -- coming back down
    96.0,  282.0, 26.0, 84.0,   1.0, -42.0,  // 69
   126.0,  298.0, 26.0, 68.0,   1.0, -34.0,  // 70 
   156.0,  298.0, 28.0, 68.0,   0.0, -34.0,  // 71
   186.0,  298.0, 30.0, 68.0,   1.0, -34.0,  // 72
   220.0,  298.0, 28.0, 68.0,   0.0, -34.0,  // 73
   250.0,  298.0, 26.0, 68.0,  -1.0, -34.0,  // 74
   278.0,  298.0, 28.0, 68.0,   0.0, -34.0,  // 75
   308.0,  298.0, 26.0, 70.0,  -1.0, -35.0,  // 76
   336.0,  296.0, 22.0, 72.0,   1.0, -36.0,  // 77
   364.0,  290.0, 18.0, 78.0,   1.0, -39.0,  // 78

// skipping a bunch for now

// row 16 -- 1428
    16.0, 1432.0, 28.0, 90.0,   0.0, -47.0,  // 79 -- in the air, same as 67
    58.0, 1428.0, 22.0, 94.0,  -3.0, -49.0,  // 80 -- start of just hanging there
    88.0, 1428.0, 28.0, 94.0,   0.0, -49.0,  // 81
   116.0, 1428.0, 30.0, 94.0,   3.0, -49.0,  // 82
   148.0, 1428.0, 34.0, 94.0,   3.0, -49.0,  // 83
   188.0, 1428.0, 26.0, 94.0,  -1.0, -49.0,  // 84
   224.0, 1428.0, 22.0, 94.0,  -3.0, -49.0,  // 85
   256.0, 1428.0, 22.0, 94.0,  -3.0, -49.0,  // 86
   286.0, 1428.0, 22.0, 94.0,  -3.0, -49.0,  // 87
   312.0, 1428.0, 22.0, 94.0,  -3.0, -49.0,  // 88 -- end just hanging there
   342.0, 1428.0, 18.0, 94.0,   1.0, -49.0,  // 89 -- start of climb up
   368.0, 1428.0, 24.0, 94.0,   6.0, -49.0,  // 90
   398.0, 1428.0, 24.0, 94.0,   6.0, -49.0,  // 91
   428.0, 1428.0, 24.0, 94.0,   6.0, -51.0,  // 92
   458.0, 1428.0, 30.0, 94.0,   9.0, -53.0,  // 93
   490.0, 1428.0, 28.0, 94.0,   8.0, -61.0,  // 94
   526.0, 1428.0, 28.0, 94.0,   8.0, -63.0,  // 95
   562.0, 1428.0, 30.0, 94.0,   9.0, -67.0,  // 96
   602.0, 1428.0, 38.0, 94.0,  13.0, -67.0,  // 97
   646.0, 1428.0, 40.0, 94.0,  12.0, -67.0,  // 98
   688.0, 1428.0, 40.0, 94.0,  10.0, -69.0,  // 99
   728.0, 1428.0, 42.0, 94.0,   9.0, -71.0,  // 100
    
   // row 17 -- 1584
     8.0, 1584.0, 36.0, 62.0,   6.0, -87.0,  // 101
    52.0, 1584.0, 34.0, 62.0,   3.0 - 1.0, -87.0,  // 102
    94.0, 1582.0, 34.0, 62.0,   3.0 - 2.0, -89.0,  // 103
   132.0, 1578.0, 38.0, 62.0,   3.0 - 3.0, -93.0,  // 104
   172.0, 1570.0, 38.0, 62.0,   1.0 - 4.0, -101.0,  // 105
   212.0, 1566.0, 40.0, 62.0,   4.0 - 4.0, -105.0,  // 106
   250.0, 1556.0, 38.0, 62.0,   1.0 - 4.0, -115.0,  // 107
   290.0, 1556.0, 38.0, 62.0,   1.0 - 4.0, -115.0,  // 108
   330.0, 1554.0, 32.0, 62.0,   0.0 - 4.0, -117.0,  // 109
   366.0, 1552.0, 30.0, 62.0,  -1.0 - 4.0, -119.0,  // 110
   400.0, 1548.0, 30.0, 62.0,  -3.0 - 4.0, -123.0,  // 111
   436.0, 1540.0, 26.0, 68.0,  -3.0 - 4.0, -128.0,  // 112
   470.0, 1534.0, 20.0, 74.0,   0.0 - 4.0, -131.0,  // 113
   504.0, 1530.0, 18.0, 78.0,   5.0 - 4.0, -133.0,  // 114 -- end of climb up (note, we end up 4.0 to far to the right)
   
// need to complete this...


};

// now define our animations
// note that horizontal movements should be multiples of 32 (possibly split over multiple animations in sequence)
// our vertical movement should be multiples of 96
animation conradAnim[] = {
    0,    0,  false,   0.0,  0.0,    //  0 - look left
    0,    6,  false,   0.0,  0.0,    //  1 - turn from looking left to looking right
    6,    6,  false,   0.0,  0.0,    //  2 - look right
    6,    0,  false,   0.0,  0.0,    //  3 - turn from looking right to looking left
    
   11,   15,  false,  30.0,  0.0,    //  4 - walk right (left foot)
    7,   10,  false,   2.0,  0.0,    //  5 - stop walking right (after left foot)
   16,   22,  false,  32.0,  0.0,    //  6 - walk right (right foot)
   
   11,   15,   true, -30.0,  0.0,    //  7 - walk left (left foot, uhm or right foot, mirrored you know)
    7,   10,   true,  -2.0,  0.0,    //  8 - stop walking left (after left foot)
   16,   22,   true, -32.0,  0.0,    //  9 - walk left (right foot)
   
   24,   31,   true,   0.0,  0.0,    // 10 - get down (facing right)
   31,   31,   true,   0.0,  0.0,    // 11 - stay down (facing right)
   32,   45,   true, 108.0,  0.0,    // 12 - roll
   45,   32,   true, 108.0,  0.0,    // 13 - reverse roll
   46,   46,   true,  20.0,  0.0,    // 14 - get up 1
   47,   49,   true,   0.0,  0.0,    // 15 - get up 2

   24,   31,  false,   0.0,  0.0,    // 16 - get down (facing left)
   31,   31,  false,   0.0,  0.0,    // 17 - stay down (facing left)
   32,   45,  false,-108.0,  0.0,    // 18 - roll
   45,   32,  false,-108.0,  0.0,    // 19 - reverse roll
   46,   46,  false, -20.0,  0.0,    // 20 - get up 1
   47,   49,  false,   0.0,  0.0,    // 21 - get up 2

   50,   66,   true,   0.0,  0.0,    // 22 - jump up (facing right)
   80,   88,   true,   0.0,  0.0,    // 23 - just hanging (facing right)
   68,   78,   true,   0.0,  0.0,    // 24 - coming back down (facing right)
   89,  114,   true,   0.0,-96.0,    // 25 - climbing up (facing right)

   50,   66,  false,   0.0,  0.0,    // 26 - jump up (facing left)
   80,   88,  false,   0.0,  0.0,    // 27 - just hanging (facing left)
   68,   78,  false,   0.0,  0.0,    // 28 - coming back down (facing left)   
   89,  114,  false,   0.0,-96.0,    // 29 - climbing up (facing left)
};

// and define our action map, the first action that matches defines our next animation
action_map conradActions[] = {
  // looking left
  CR_ANIM_LOOK_LEFT, CR_ANIM_TURN_LEFT_TO_RIGHT, { GLFW_KEY_RIGHT , 0, 0, 0, 0},                  // turn left to right if our right key is pressed
  CR_ANIM_LOOK_LEFT, CR_ANIM_WALK_LEFT_1, { GLFW_KEY_LEFT, 0, 0, 0, 0},                           // start walking right
  CR_ANIM_LOOK_LEFT, CR_ANIM_GET_DOWN_LEFT, { GLFW_KEY_DOWN, 0, 0, 0, 0},                         // start getting down facing left
  CR_ANIM_LOOK_LEFT, CR_ANIM_JUMP_UP_LEFT, { GLFW_KEY_UP, 0, 0, 0, 0},                            // jump up
  CR_ANIM_LOOK_LEFT, CR_ANIM_LOOK_LEFT, { 0, 0, 0, 0, 0},                                         // keep looking left if no key is pressed
  CR_ANIM_TURN_LEFT_TO_RIGHT, CR_ANIM_LOOK_RIGHT, { 0, 0, 0, 0, 0},                               // turn to looking right once finished

  // looking right
  CR_ANIM_LOOK_RIGHT, CR_ANIM_TURN_RIGHT_TO_LEFT, { GLFW_KEY_LEFT , 0, 0, 0, 0},                  // turn right to left if our left key is pressed
  CR_ANIM_LOOK_RIGHT, CR_ANIM_WALK_RIGHT_1, { GLFW_KEY_RIGHT, 0, 0, 0, 0},                        // start walking right
  CR_ANIM_LOOK_RIGHT, CR_ANIM_GET_DOWN_RIGHT, { GLFW_KEY_DOWN, 0, 0, 0, 0},                       // start getting down facing right
  CR_ANIM_LOOK_RIGHT, CR_ANIM_JUMP_UP_RIGHT, { GLFW_KEY_UP, 0, 0, 0, 0},                          // jump up
  CR_ANIM_LOOK_RIGHT, CR_ANIM_LOOK_RIGHT, { 0, 0, 0, 0, 0},                                       // keep looking right if no key is pressed
  CR_ANIM_TURN_RIGHT_TO_LEFT, CR_ANIM_LOOK_LEFT, { 0, 0, 0, 0, 0},                                // turn to looking left once finished
  
  // walking right
  CR_ANIM_WALK_RIGHT_1, CR_ANIM_WALK_RIGHT_2, { GLFW_KEY_RIGHT, 0, 0, 0, 0},                      // keep walking right
  CR_ANIM_WALK_RIGHT_1, CR_ANIM_STOP_WALK_RIGHT, { 0, 0, 0, 0, 0},                                // stop walking right if no key is pressed
  CR_ANIM_WALK_RIGHT_2, CR_ANIM_WALK_RIGHT_1, { GLFW_KEY_RIGHT, 0, 0, 0, 0},                      // keep walking right
  CR_ANIM_WALK_RIGHT_2, CR_ANIM_LOOK_RIGHT, { 0, 0, 0, 0, 0},                                     // finished walking right if no key is pressed
  CR_ANIM_STOP_WALK_RIGHT, CR_ANIM_LOOK_RIGHT, { 0, 0, 0, 0, 0},                                  // finished walking right if no key is pressed

  // walking left
  CR_ANIM_WALK_LEFT_1, CR_ANIM_WALK_LEFT_2, { GLFW_KEY_LEFT, 0, 0, 0, 0},                         // keep walking left
  CR_ANIM_WALK_LEFT_1, CR_ANIM_STOP_WALK_LEFT, { 0, 0, 0, 0, 0},                                  // stop walking left if no key is pressed
  CR_ANIM_WALK_LEFT_2, CR_ANIM_WALK_LEFT_1, { GLFW_KEY_LEFT, 0, 0, 0, 0},                         // keep walking left
  CR_ANIM_WALK_LEFT_2, CR_ANIM_LOOK_LEFT, { 0, 0, 0, 0, 0},                                       // finished walking left if no key is pressed
  CR_ANIM_STOP_WALK_LEFT, CR_ANIM_LOOK_LEFT, { 0, 0, 0, 0, 0},                                    // finished walking left if no key is pressed
  
  // getting down (facing left)
  CR_ANIM_GET_DOWN_LEFT, CR_ANIM_STAY_DOWN_LEFT, { 0, 0, 0, 0, 0},                                // switch to stay down, then further actions go from there
  CR_ANIM_STAY_DOWN_LEFT, CR_ANIM_ROLL_LEFT, { GLFW_KEY_DOWN, GLFW_KEY_LEFT, 0, 0, 0},            // roll left
  CR_ANIM_STAY_DOWN_LEFT, CR_ANIM_REVROLL_LEFT, { GLFW_KEY_DOWN, GLFW_KEY_RIGHT, 0, 0, 0},        // reverse roll left
  CR_ANIM_STAY_DOWN_LEFT, CR_ANIM_STAY_DOWN_LEFT, { GLFW_KEY_DOWN, 0, 0, 0, 0},                   // stay down facing left
  CR_ANIM_STAY_DOWN_LEFT, CR_ANIM_GET_UP_LEFT_2, { 0, 0, 0, 0, 0},                                // get back up
  CR_ANIM_ROLL_LEFT, CR_ANIM_ROLL_LEFT, { GLFW_KEY_DOWN, GLFW_KEY_LEFT, 0, 0, 0},                 // keep rolling left
  CR_ANIM_ROLL_LEFT, CR_ANIM_STAY_DOWN_LEFT, { 0, 0, 0, 0, 0},                                    // anything else switch to stay down, then further actions go from there
  CR_ANIM_REVROLL_LEFT, CR_ANIM_REVROLL_LEFT, { GLFW_KEY_DOWN, GLFW_KEY_RIGHT, 0, 0, 0},          // keep rolling left
  CR_ANIM_REVROLL_LEFT, CR_ANIM_STAY_DOWN_LEFT, { 0, 0, 0, 0, 0},                                 // anything else switch to stay down, then further actions go from there
  CR_ANIM_GET_UP_LEFT_2, CR_ANIM_LOOK_LEFT, { 0, 0, 0, 0, 0},                                     // and switch to looking left

  // getting down (facing rigth)
  CR_ANIM_GET_DOWN_RIGHT, CR_ANIM_STAY_DOWN_RIGHT, { 0, 0, 0, 0, 0},                              // switch to stay down, then further actions go from there
  CR_ANIM_STAY_DOWN_RIGHT, CR_ANIM_ROLL_RIGHT, { GLFW_KEY_DOWN, GLFW_KEY_RIGHT, 0, 0, 0},         // roll left
  CR_ANIM_STAY_DOWN_RIGHT, CR_ANIM_REVROLL_RIGHT, { GLFW_KEY_DOWN, GLFW_KEY_LEFT, 0, 0, 0},       // reverse roll left
  CR_ANIM_STAY_DOWN_RIGHT, CR_ANIM_STAY_DOWN_RIGHT, { GLFW_KEY_DOWN, 0, 0, 0, 0},                 // stay down facing left
  CR_ANIM_STAY_DOWN_RIGHT, CR_ANIM_GET_UP_RIGHT_2, { 0, 0, 0, 0, 0},                              // get back up
  CR_ANIM_ROLL_RIGHT, CR_ANIM_ROLL_RIGHT, { GLFW_KEY_DOWN, GLFW_KEY_RIGHT, 0, 0, 0},              // keep rolling left
  CR_ANIM_ROLL_RIGHT, CR_ANIM_STAY_DOWN_RIGHT, { 0, 0, 0, 0, 0},                                  // anything else switch to stay down, then further actions go from there
  CR_ANIM_REVROLL_RIGHT, CR_ANIM_REVROLL_RIGHT, { GLFW_KEY_DOWN, GLFW_KEY_LEFT, 0, 0, 0},         // keep rolling left
  CR_ANIM_REVROLL_RIGHT, CR_ANIM_STAY_DOWN_RIGHT, { 0, 0, 0, 0, 0},                               // anything else switch to stay down, then further actions go from there
  CR_ANIM_GET_UP_RIGHT_2, CR_ANIM_LOOK_RIGHT, { 0, 0, 0, 0, 0},                                   // and switch to looking right
  
  // jumping (facing left)
  CR_ANIM_JUMP_UP_LEFT, CR_ANIM_COME_DOWN_LEFT, { GLFW_KEY_DOWN, 0, 0, 0, 0 },                    // comming back down
  CR_ANIM_JUMP_UP_LEFT, CR_ANIM_CLIMB_UP_LEFT, { GLFW_KEY_UP, 0, 0, 0, 0 },                       // climbing up
  CR_ANIM_JUMP_UP_LEFT, CR_ANIM_JUST_HANGING_LEFT, { 0, 0, 0, 0, 0 },                             // just hanging...
  CR_ANIM_JUST_HANGING_LEFT, CR_ANIM_COME_DOWN_LEFT, { GLFW_KEY_DOWN, 0, 0, 0, 0 },               // comming back down
  CR_ANIM_JUST_HANGING_LEFT, CR_ANIM_CLIMB_UP_LEFT, { GLFW_KEY_UP, 0, 0, 0, 0 },                  // climbing up
  CR_ANIM_JUST_HANGING_LEFT, CR_ANIM_JUST_HANGING_LEFT, { 0, 0, 0, 0, 0 },                        // just hanging...
  CR_ANIM_COME_DOWN_LEFT, CR_ANIM_LOOK_LEFT, { 0, 0, 0, 0, 0 },                                   // and looking left again
  CR_ANIM_CLIMB_UP_LEFT, CR_ANIM_LOOK_LEFT, { 0, 0, 0, 0, 0 },                                    // and looking left again

  // jumping (facing right)
  CR_ANIM_JUMP_UP_RIGHT, CR_ANIM_COME_DOWN_RIGHT, { GLFW_KEY_DOWN, 0, 0, 0, 0 },                  // comming back down
  CR_ANIM_JUMP_UP_RIGHT, CR_ANIM_CLIMB_UP_RIGHT, { GLFW_KEY_UP, 0, 0, 0, 0 },                     // climbing up
  CR_ANIM_JUMP_UP_RIGHT, CR_ANIM_JUST_HANGING_RIGHT, { 0, 0, 0, 0, 0 },                           // just hanging...
  CR_ANIM_JUST_HANGING_RIGHT, CR_ANIM_COME_DOWN_RIGHT, { GLFW_KEY_DOWN, 0, 0, 0, 0 },             // comming back down
  CR_ANIM_JUST_HANGING_RIGHT, CR_ANIM_CLIMB_UP_RIGHT, { GLFW_KEY_UP, 0, 0, 0, 0 },                // climbing up
  CR_ANIM_JUST_HANGING_RIGHT, CR_ANIM_JUST_HANGING_RIGHT, { 0, 0, 0, 0, 0 },                      // just hanging...
  CR_ANIM_COME_DOWN_RIGHT, CR_ANIM_LOOK_RIGHT, { 0, 0, 0, 0, 0 },                                 // and looking left again
  CR_ANIM_CLIMB_UP_RIGHT, CR_ANIM_LOOK_RIGHT, { 0, 0, 0, 0, 0 },                                  // and looking left again
};

#define MAX_ACTIONS sizeof(conradActions) / sizeof(action_map)

double    lastAnimSecs = 0.0;
GLint     currentAnim = CR_ANIM_LOOK_LEFT;
GLint     currentSprite = 0;
vec3      currentPos = { 0.0, 400.0, 0.0 };

//////////////////////////////////////////////////////////
// error handling

// sets our error callback method which is modelled after 
// GLFWs error handler so you can use the same one
void engineSetErrorCallback(EngineError pCallback) {
  engineErrCallback = pCallback;
  
  // Set our callbacks in our support libraries
  shaderSetErrorCallback(engineErrCallback);
  tsSetErrorCallback(engineErrCallback);
  spSetErrorCallback(engineErrCallback);
};

//////////////////////////////////////////////////////////
// keyboard handling

// sets a callback that allows us to test if a key is
// pressed
void engineSetKeyPressedCallback(EngineKeyPressed pCallback) {
  engineKeyPressedCallback = pCallback;
};

//////////////////////////////////////////////////////////
// fonts
void load_font() {
  // we start with creating a font context that tells us about the font we'll be rendering
  fs = gl3fonsCreate(512, 512, FONS_ZERO_TOPLEFT);
  if (fs != NULL) {
    // then we load our font
    font = fonsAddFont(fs, "sans", "DroidSerif-Regular.ttf");
    if (font != FONS_INVALID) {
      // setup our font
      fonsSetColor(fs, gl3fonsRGBA(255,255,255,255)); // white
      fonsSetSize(fs, 32.0f); // 32 point font
      fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_TOP); // left/top aligned
      fonsVertMetrics(fs, NULL, NULL, &lineHeight);
    } else {
      engineErrCallback(-201, "Couldn't load DroidSerif-Regular.ttf");       
    };
  } else {
    engineErrCallback(-200, "Couldn't create our font context");
  };
};

void unload_font() {
  if (fs != NULL) {
    gl3fonsDelete(fs);
    fs = NULL;
  };
};

//////////////////////////////////////////////////////////
// Shaders

// load contents of a file
// return NULL on failure
// returns string on success, calling function is responsible for freeing the text
char* loadFile(const char* pFileName) {
  char  error[1024];
  char* result = NULL;

  // read "binary", we simply keep our newlines as is
  FILE* file = fopen(pFileName,"rb");
  if (file == NULL) {
    sprintf(error,"Couldn't open %s",pFileName);
    engineErrCallback(-100, error);
  } else {
    long size;
    
    // find the end of the file to get its size
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // allocate space for our data
    result = (char*) malloc(size+1);
    if (result == NULL) {
      sprintf(error,"Couldn't allocate memory buffer for %li bytes",size);
      engineErrCallback(-101, error);    
    } else {
      // and load our text
      fread(result, 1, size, file);
      result[size] = 0;
    };
    
    fclose(file);
  };
  
  return result;
};

void load_shaders() {
  // load our tilemap shader
  tsSetLoadFileFunc(loadFile);
  tsLoad(&ts);
  
  // load our spritesheet shader
  spSetLoadFileFunc(loadFile);
  spLoad(&sp);
};

void unload_shaders() {
  tsUnload(&ts);
  spUnload(&sp);
};

//////////////////////////////////////////////////////////
// Objects

void setTexture(GLuint pTexture, GLint pFilter, GLint pWrap) {
  glBindTexture(GL_TEXTURE_2D, pTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, pFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, pFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, pWrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, pWrap);  
};

void load_objects() {
	int x, y, comp;
  unsigned char * data;

  // we start with creating our vertex array object, even though we're not doing much with it, OpenGL still requires one for state
  glGenVertexArrays(1, &VAO);
  
  // Need to create our texture objects
  glGenTextures(TEXT_COUNT, textures);
  
  ////////////////////////////////////////////////////////////////////////////////////////////////
  // now load in our map texture into textures[TEXT_MAPDATA]
	setTexture(textures[TEXT_MAPDATA], GL_LINEAR, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 40, 40, 0, GL_RED, GL_UNSIGNED_BYTE, mapdata);
  ts.mapdataTexture = textures[TEXT_MAPDATA];
  
  // and we load our tiled map into textures[TEXT_TILEDATA]
	data = stbi_load("desert256.png", &x, &y, &comp, 4);
  if (data == 0) {
    engineErrCallback(-1, "Couldn't load desert256.png");
  } else {
  	setTexture(textures[TEXT_TILEDATA], GL_LINEAR, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    ts.tileTexture = textures[TEXT_TILEDATA];
		
		stbi_image_free(data);
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////
  // now load in our sprite texture into textures[TEXT_SPRITEDATA]
	data = stbi_load("conrad.png", &x, &y, &comp, 4);
  if (data == 0) {
    engineErrCallback(-1, "Couldn't load conrad.png");
  } else {
  	setTexture(textures[TEXT_SPRITEDATA], GL_LINEAR, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    sp.spriteTexture = textures[TEXT_SPRITEDATA];
    sp.spriteWidth = x;
    sp.spriteHeight = y;
		
		stbi_image_free(data);
  };

  // normally we would load this from a file or another source
  // copying from a memory buffer like this is a bit wasteful.
  spAddSprites(&sp, conradSpriteSheet, sizeof(conradSpriteSheet) / sizeof(sprite));

  // and lets be nice and unbind...
	glBindTexture(GL_TEXTURE_2D, 0);

  // and clear our selected vertex array object
  glBindVertexArray(0);
};

void unload_objects() {
  sp.spriteTexture = 0;
  ts.tileTexture = 0;
  ts.mapdataTexture = 0;
  
  glDeleteTextures(TEXT_COUNT, textures);
  glDeleteVertexArrays(1, &VAO);
};

//////////////////////////////////////////////////////////
// Main engine

// engineLoad loads any data that we need to load before we can start outputting stuff
void engineLoad() {
  // load our font
  load_font();
    
  // load, compile and link our shader(s)
  load_shaders();
  
  // load our objects
  load_objects();
  
  // init our view matrix (for now just an identity matrix)
  mat4Identity(&view);  
};

// engineUnload unloads and frees up any data associated with our engine
void engineUnload() {
  // lets be nice and cleanup
  unload_shaders();
  unload_objects();
  unload_font();
};

// engineUpdate is called to handle any updates of our data
// pSecondsPassed is the number of seconds passed since our application was started.
void engineUpdate(double pSecondsPassed) {
  double delta;
  bool   animReset = false;
  
  // update our animation
  delta = (pSecondsPassed - lastAnimSecs);
  if (delta > (1.0 / 12.0)) { // roughly aim for 12 frames per second
    lastAnimSecs = pSecondsPassed;
    
    if (conradAnim[currentAnim].lastSprite < conradAnim[currentAnim].firstSprite) {
      // move backwards one sprite
      currentSprite--;
      if (currentSprite < conradAnim[currentAnim].lastSprite) {
        currentSprite = conradAnim[currentAnim].firstSprite;
        animReset = true;
      };
    } else {      
      // move forewards one sprite
      currentSprite++;
      if (currentSprite > conradAnim[currentAnim].lastSprite) {
        currentSprite = conradAnim[currentAnim].firstSprite;
        animReset = true;
      };
    };
    
    // if our animation was reset we have a chance to change the animation
    if (animReset) {
      int nextAnim = -1;
      
      // if our current animation was played forward, we add our movement
      if (conradAnim[currentAnim].firstSprite<=conradAnim[currentAnim].lastSprite) {
        currentPos.x += conradAnim[currentAnim].moveX * SPRITE_SCALE;
        currentPos.y += conradAnim[currentAnim].moveY * SPRITE_SCALE;        
      };
      
      // check our action map
      for (int a = 0; a < MAX_ACTIONS && nextAnim == -1; a++) {
        if (conradActions[a].animationEnding == currentAnim) {
          // assume this will be our next animation until proven differently
          nextAnim = conradActions[a].startAnimation;
                    
          // check if we're missing any keys
          for (int k = 0; k < 5 && nextAnim != -1; k++) {
            int key = conradActions[a].keys[k];
            if (key == 0) {
              // no need to check further
              k = 4;
            } else if (!engineKeyPressedCallback(key)) {
              nextAnim = -1;
            };
          };
        };
      };

      if (nextAnim == -1) {
        engineErrCallback(0, "Missing animation from %i", currentAnim);
      };
      
      // if -1  We're missing something
      currentAnim = nextAnim == -1 ? 0 : nextAnim;
      currentSprite = conradAnim[currentAnim].firstSprite;
      
      // if our new animation is going to be played in reverse, we subtract our movement
      if (conradAnim[currentAnim].firstSprite>conradAnim[currentAnim].lastSprite) {
        currentPos.x -= conradAnim[currentAnim].moveX * SPRITE_SCALE;
        currentPos.y -= conradAnim[currentAnim].moveY * SPRITE_SCALE;        
      };
    };
  };
  
  // update our frame counter
  frames += 1.0f;
  delta = pSecondsPassed - lastsecs;
  fps = (frames - lastframes) / delta;

  if (frames - lastframes > 100.0f) {
    // reset every 100 frames
    lastsecs = pSecondsPassed;
    lastframes = frames;
  };
};

// engineRender is called to render our stuff
void engineRender(int pWidth, int pHeight) {;
  mat4 mvp, invmvp, projection, modelview;
  vec3 tmpvector;
  float ratio, left, top;
  int a, b, i;
  
  // select our default VAO so we can render stuff that doesn't need our VAO
  glBindVertexArray(VAO);
  
  // enable our depth test, with this disabled our second triangle paints over our first..
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  // set our model view projection matrix
  ratio = pWidth / (float) pHeight;

  // init our projection matrix, note that we've got positive Y pointing down
  mat4Identity(&projection);
  mat4Ortho(&projection, -ratio * 500.0, ratio * 500.0, 500.0f, -500.0f, 1.0f, -1.0f);
          
  // draw our tiled background
//  tsRender(&ts, &projection, &view);
  
  // draw our character
  mat4Copy(&modelview, &view);
  mat4Translate(&modelview, &currentPos);
  sp.spriteScale = SPRITE_SCALE;
  spRender(&sp, &projection, &modelview, currentSprite, conradAnim[currentAnim].flip, false);
  
  // unset stuff
  glBindVertexArray(0);
  glUseProgram(0);
  
  ////////////////////////////////////
  // UI
    
  // change our state a little
  glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);  
  
  // now render our FPS
	if ((fs != NULL) && (font != FONS_INVALID)) {
    char info[256];
    
    // we can use our same projection matrix
    gl3fonsProjection(fs, (GLfloat *)projection.m);

    // what text shall we draw?
    sprintf(info,"FPS: %0.1f, anim = %i, index = %i", fps, currentAnim, currentSprite);
        
    // and draw some text
    fonsDrawText(fs, -ratio * 500.0f, 460.0f, info, NULL);
  };
};

void engineKeyPressed(int pKey) {
//  pushKey(pKey);
};


