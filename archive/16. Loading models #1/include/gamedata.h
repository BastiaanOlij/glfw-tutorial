/********************************************************
 * This file contains compilable game data and should
 * be included in our main source file.
 *
 * Normally data like this would be saved in some format
 * and loaded so multiple maps and characters can be used
 * but for our tutorial purposes we're simply including
 * this as source
 * 
 ********************************************************/

//////////////////////////////////////////////////////////
// Our map data consists of an indexed array to our tiles
// that we'll render and an 'interaction map' that
// overlays this and allows us to know where our character
// can go

// map data for our background
unsigned char mapdata[800] = {
  96,97,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,45,
  112,113,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,60,61,
  128,129,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,45,
  144,145,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,60,61,
  64,81,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,45,
  80,81,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,60,61,
  96,97,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,45,
  112,113,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,60,61,
  128,129,0,0,0,0,0,0,0,66,67,68,69,70,71,72,73,74,75,76,77,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,45,
  144,145,0,0,0,0,0,0,0,82,83,84,85,86,87,88,89,90,91,92,93,243,244,245,89,87,88,247,20,21,0,0,0,0,0,0,0,0,60,61,
  64,81,0,0,0,0,0,0,0,98,99,100,101,102,103,104,105,106,107,108,109,110,33,34,105,103,104,105,36,37,0,0,0,0,0,0,0,0,44,45,
  80,81,0,0,0,0,0,0,0,114,115,116,117,118,119,120,121,122,123,124,125,126,49,50,121,119,120,121,52,53,0,0,0,0,0,0,0,0,60,61,
  144,145,0,165,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,141,142,65,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,45,
  162,163,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,81,0,0,0,0,77,0,66,67,68,69,70,0,0,0,0,60,61,
  178,179,18,92,93,90,91,19,20,21,0,0,0,0,0,0,0,0,0,0,0,60,113,0,0,0,0,0,0,82,83,84,85,86,90,91,92,93,94,95,
  194,195,34,108,103,106,107,35,36,37,0,0,0,0,0,0,0,0,0,0,0,44,129,0,0,0,0,0,0,98,99,100,101,102,106,107,108,109,110,111,
  210,211,50,118,117,122,123,124,52,53,0,0,0,0,0,0,0,0,0,0,0,60,145,0,0,0,0,0,0,114,115,116,117,118,122,123,124,125,126,127,
  208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,130,131,132,133,134,44,81,140,0,0,0,0,0,0,0,0,0,0,0,0,188,141,142,191,
  224,225,0,0,0,0,0,0,0,0,0,235,236,237,238,227,228,229,230,204,205,206,97,204,205,0,0,0,228,229,230,231,232,233,234,235,204,205,206,207,
  240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,243,244,245,246,220,221,222,223,220,243,244,245,246,247,248,249,250,248,249,250,251,220,221,222,223
};

// interaction data:
// 0 = can't stand here
// 1 = can stand/walk here
// 2 = can stand/climb up facing left
// 3 = can stand/climb up facing right
unsigned char interactiondata[800] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,1,1,1,1,1,1,1,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1,1,1,1,1,1,1,1,1,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0
};

// some zero terminated arrays for checking allowed actions
unsigned char idCanWalk[] = { 1, 2, 3, 0};
unsigned char idCanCrouch[] = { 1, 0 };
unsigned char idCanClimbLeft[] = { 2, 0 };
unsigned char idCanClimbRight[] = { 3, 0 };

//////////////////////////////////////////////////////////
// Below we define our character animations

// we start with an enum that identifies all our
// animations, our arrays with data should match so we can
// use these enums as indexes into our arrays
enum conrad_animations {
  CR_ANIM_LOOK_LEFT,
  CR_ANIM_TURN_LEFT_TO_RIGHT,
  CR_ANIM_LOOK_RIGHT,
  CR_ANIM_TURN_RIGHT_TO_LEFT,

  CR_ANIM_WALK_RIGHT_1,
  CR_ANIM_STOP_WALK_RIGHT,
  CR_ANIM_WALK_RIGHT_2,

  CR_ANIM_WALK_LEFT_1,
  CR_ANIM_STOP_WALK_LEFT,
  CR_ANIM_WALK_LEFT_2,
  
  CR_ANIM_GET_DOWN_RIGHT,
  CR_ANIM_STAY_DOWN_RIGHT,
  CR_ANIM_ROLL_RIGHT,
  CR_ANIM_REVROLL_RIGHT,
  CR_ANIM_GET_UP_RIGHT_1,
  CR_ANIM_GET_UP_RIGHT_2,

  CR_ANIM_GET_DOWN_LEFT,
  CR_ANIM_STAY_DOWN_LEFT,
  CR_ANIM_ROLL_LEFT,
  CR_ANIM_REVROLL_LEFT,
  CR_ANIM_GET_UP_LEFT_1,
  CR_ANIM_GET_UP_LEFT_2,
  
  CR_ANIM_JUMP_UP_RIGHT,
  CR_ANIM_JUST_HANGING_RIGHT,
  CR_ANIM_COME_DOWN_RIGHT,
  CR_ANIM_CLIMB_UP_RIGHT,
  CR_ANIM_CLIMB_DOWN_RIGHT,

  CR_ANIM_JUMP_UP_LEFT,
  CR_ANIM_JUST_HANGING_LEFT,
  CR_ANIM_COME_DOWN_LEFT,
  CR_ANIM_CLIMB_UP_LEFT,
  CR_ANIM_CLIMB_DOWN_LEFT,
  
  CR_ANIM_FALLING_LEFT,
  CR_ANIM_FALLING_RIGHT,

  CR_ANIM_COUNT,
  CR_END
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
   
   169.0,    0.0, 28.0, 78.0,  -1.0, -39.0,  //  7 -- stop walking right from 15
   198.0,    0.0, 26.0, 78.0,  -1.0, -39.0,  //  8
   224.0,    0.0, 22.0, 78.0,   1.0, -39.0,  //  9 
   246.0,    0.0, 18.0, 78.0,   1.0, -39.0,  // 10 -- finish stop walking right
  
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
    66.0,  274.0, 30.0, 92.0,  -3.0, -47.0 + 64.0,  // 68 -- coming back down
    96.0,  282.0, 26.0, 84.0,   1.0, -42.0 + 64.0,  // 69
   126.0,  298.0, 26.0, 68.0,   1.0, -34.0 + 64.0,  // 70 
   156.0,  298.0, 28.0, 68.0,   0.0, -34.0 + 64.0,  // 71
   186.0,  298.0, 30.0, 68.0,   1.0, -34.0 + 64.0,  // 72
   220.0,  298.0, 28.0, 68.0,   0.0, -34.0 + 64.0,  // 73
   250.0,  298.0, 26.0, 68.0,  -1.0, -34.0 + 64.0,  // 74
   278.0,  298.0, 28.0, 68.0,   0.0, -34.0 + 64.0,  // 75
   308.0,  298.0, 26.0, 70.0,  -1.0, -35.0 + 64.0,  // 76
   336.0,  296.0, 22.0, 72.0,   1.0, -36.0 + 64.0,  // 77
   364.0,  290.0, 18.0, 78.0,   1.0, -39.0 + 64.0,  // 78

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
   
  // and so forth....
};

// define arrays of key combos that we use to check for input
int keysRight[]     = { GLFW_KEY_RIGHT, 0 };
int keysLeft[]      = { GLFW_KEY_LEFT, 0 };
int keysDown[]      = { GLFW_KEY_DOWN, 0 };
int keysUp[]        = { GLFW_KEY_UP, 0 };
int keysDownRight[] = { GLFW_KEY_DOWN, GLFW_KEY_RIGHT, 0 };
int keysDownLeft[]  = { GLFW_KEY_DOWN, GLFW_KEY_LEFT, 0 };

// define our movement checks
move_map conradCanWalkLeft[] = {
  -1,  0, idCanWalk,
   0,  0, NULL,
};

move_map conradCanWalkRight[] = {
   1,  0, idCanWalk,
   0,  0, NULL,
};

move_map conradCanRollLeft[] = {
  -1,  0, idCanWalk,
  -2,  0, idCanWalk,
  -3,  0, idCanWalk,
  -4,  0, idCanWalk,
   0,  0, NULL,
};

move_map conradCanRollRight[] = {
   1,  0, idCanWalk,
   2,  0, idCanWalk,
   3,  0, idCanWalk,
   4,  0, idCanWalk,
   0,  0, NULL,
};

move_map conradClimbLeft[] = {
   0, -3, idCanClimbLeft,                                                       // note, we're going 5 cells up, but we've already moved 2
   0,  0, NULL,
};

move_map conradClimbRight[] = {
   0, -3, idCanClimbRight,                                                       // note, we're going 5 cells up, but we've already moved 2
   0,  0, NULL,
};

move_map conradCanCrouch[] = {
   0,  0, idCanCrouch,
   0,  0, NULL,
};

move_map conradCanGoDownLeft[] = {
   0,  0, idCanClimbLeft,
   0,  0, NULL,
};

move_map conradCanGoDownRight[] = {
   0,  0, idCanClimbRight,
   0,  0, NULL,
};

move_map conradCanStand[] = {
   0,  0, idCanWalk,
   0,  0, NULL,
};

// Now we define all the followup actions for each of our actions
action_map conradLookLeftActions[] = {
  CR_ANIM_TURN_LEFT_TO_RIGHT, keysRight, conradCanStand,                        // turn left to right if our right key is pressed
  CR_ANIM_WALK_LEFT_1, keysLeft, conradCanWalkLeft,                             // start walking left
  CR_ANIM_GET_DOWN_LEFT, keysDown, conradCanCrouch,                             // start getting down facing left
  CR_ANIM_CLIMB_DOWN_LEFT, keysDown, conradCanGoDownLeft,                       // start climbing down facing left
  CR_ANIM_JUMP_UP_LEFT, keysUp, conradCanStand,                                 // jump up facing left
  CR_ANIM_LOOK_LEFT, NULL, conradCanStand,                                      // keep looking left if no key is pressed
  CR_ANIM_FALLING_LEFT, NULL, NULL,                                             // must be falling..
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradLeftToRightActions[] = {
  CR_ANIM_LOOK_RIGHT, NULL, NULL,                                               // turn to looking right once finished  
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradLookRightActions[] = {
  CR_ANIM_TURN_RIGHT_TO_LEFT, keysLeft, NULL,                                   // turn right to left if our left key is pressed
  CR_ANIM_WALK_RIGHT_1, keysRight, conradCanWalkRight,                          // start walking right
  CR_ANIM_GET_DOWN_RIGHT, keysDown, conradCanCrouch,                            // start getting down facing right
  CR_ANIM_CLIMB_DOWN_RIGHT, keysDown, conradCanGoDownRight,                     // start climbing down facing right
  CR_ANIM_JUMP_UP_RIGHT, keysUp, NULL,                                          // jump up facing right
  CR_ANIM_LOOK_RIGHT, NULL, conradCanStand,                                     // keep looking right if no key is pressed
  CR_ANIM_FALLING_RIGHT, NULL, conradCanStand,                                  // must be falling..
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradRightToLeftAction[] = {
  CR_ANIM_LOOK_LEFT, NULL, NULL,                                                // turn to looking left once finished  
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradWalkRight1Actions[] = {
  CR_ANIM_WALK_RIGHT_2, keysRight, conradCanWalkRight,                          // keep walking right
  CR_ANIM_STOP_WALK_RIGHT, NULL, NULL,                                          // stop walking right if no key is pressed
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradWalkRight2Actions[] = {
  CR_ANIM_WALK_RIGHT_1, keysRight, conradCanWalkRight,                          // keep walking right
  CR_ANIM_LOOK_RIGHT, NULL, NULL,                                               // finished walking right if no key is pressed  
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradWalkRightStopActions[] = {
  CR_ANIM_LOOK_RIGHT, NULL, NULL,                                               // finished walking right if no key is pressed
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradWalkLeft1Actions[] = {
  CR_ANIM_WALK_LEFT_2, keysLeft, conradCanWalkLeft,                             // keep walking left
  CR_ANIM_STOP_WALK_LEFT, NULL, NULL,                                           // stop walking left if no key is pressed
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradWalkLeft2Actions[] = {
  CR_ANIM_WALK_LEFT_1, keysLeft, conradCanWalkLeft,                             // keep walking left
  CR_ANIM_LOOK_LEFT, NULL, NULL,                                                // finished walking left if no key is pressed
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradWalkLeftStopActions[] = {
  CR_ANIM_LOOK_LEFT, NULL, NULL,                                                // finished walking left if no key is pressed  
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradGetDownLeftActions[] = {
  CR_ANIM_STAY_DOWN_LEFT, NULL, NULL,                                           // switch to stay down, then further actions go from there
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradStayDownLeftActions[] = {
  CR_ANIM_ROLL_LEFT, keysDownLeft, conradCanRollLeft,                           // roll left
  CR_ANIM_REVROLL_LEFT, keysDownRight, conradCanRollRight,                      // reverse roll right
  CR_ANIM_STAY_DOWN_LEFT, keysDown, NULL,                                       // stay down facing left
  CR_ANIM_GET_UP_LEFT_2, NULL, NULL,                                            // get back up
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradRollLeftActions[] = {
  CR_ANIM_ROLL_LEFT, keysDownLeft, conradCanRollLeft,                           // keep rolling left
  CR_ANIM_STAY_DOWN_LEFT, NULL, NULL,                                           // anything else switch to stay down, then further actions go from there
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradRevRollLeftActions[] = {
  CR_ANIM_REVROLL_LEFT, keysDownRight, conradCanRollRight,                      // keep rolling right
  CR_ANIM_STAY_DOWN_LEFT, NULL, NULL,                                           // anything else switch to stay down, then further actions go from there
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradGetUp2LeftActions[] = {
  CR_ANIM_LOOK_LEFT, NULL, NULL,                                                // and switch to looking left
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradGetDownRightActions[] = {
  CR_ANIM_STAY_DOWN_RIGHT, NULL, NULL,                                          // switch to stay down, then further actions go from there
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradStayDownRightActions[] = {
  CR_ANIM_ROLL_RIGHT, keysDownRight, conradCanRollRight,                        // roll right
  CR_ANIM_REVROLL_RIGHT, keysDownLeft, conradCanRollLeft,                       // reverse roll left
  CR_ANIM_STAY_DOWN_RIGHT, keysDown, NULL,                                      // stay down facing left
  CR_ANIM_GET_UP_RIGHT_2, NULL, NULL,                                           // get back up
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradRollRightActions[] = {
  CR_ANIM_ROLL_RIGHT, keysDownRight, conradCanRollRight,                        // keep rolling right
  CR_ANIM_STAY_DOWN_RIGHT, NULL, NULL,                                          // anything else switch to stay down, then further actions go from there
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradRevRollRightActions[] = {
  CR_ANIM_REVROLL_RIGHT, keysDownLeft, conradCanRollLeft,                       // keep rolling left
  CR_ANIM_STAY_DOWN_RIGHT, NULL, NULL,                                          // anything else switch to stay down, then further actions go from there
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradGetUp2RightActions[] = {
  CR_ANIM_LOOK_RIGHT, NULL, NULL,                                               // and switch to looking right 
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradJumpUpRightActions[] = {
  CR_ANIM_COME_DOWN_RIGHT, keysDown, NULL,                                      // coming back down
  CR_ANIM_CLIMB_UP_RIGHT, keysUp, conradClimbRight,                             // climbing up
  CR_ANIM_JUST_HANGING_RIGHT, NULL, conradClimbRight,                           // just hanging...
  CR_ANIM_COME_DOWN_RIGHT, NULL, NULL,                                          // must come down
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradJustHangRightActions[] = {
  CR_ANIM_COME_DOWN_RIGHT, keysDown, NULL,                                      // coming back down
  CR_ANIM_CLIMB_UP_RIGHT, keysUp, NULL,                                         // climbing up
  CR_ANIM_JUST_HANGING_RIGHT, NULL, NULL,                                       // just hanging...
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradComeDownRightActions[] = {
  CR_ANIM_LOOK_RIGHT, NULL, NULL,                                               // and looking left again  
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradClimbUpRightActions[] = {
  CR_ANIM_LOOK_RIGHT, NULL, NULL,                                               // and looking left again
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradClimbDownRightActions[] = {
  CR_ANIM_JUST_HANGING_RIGHT, NULL, NULL,                                       // just hanging... (for one frame)  
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradJumpUpLeftActions[] = {
  CR_ANIM_COME_DOWN_LEFT, keysDown, NULL,                                       // coming back down
  CR_ANIM_CLIMB_UP_LEFT, keysUp, conradClimbLeft,                               // climbing up
  CR_ANIM_JUST_HANGING_LEFT, NULL, conradClimbLeft,                             // just hanging...
  CR_ANIM_COME_DOWN_LEFT, NULL, NULL,                                           // must come down
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradJustHangLeftActions[] = {
  CR_ANIM_COME_DOWN_LEFT, keysDown, NULL,                                       // coming back down
  CR_ANIM_CLIMB_UP_LEFT, keysUp, NULL,                                          // climbing up
  CR_ANIM_JUST_HANGING_LEFT, NULL, NULL,                                        // just hanging...
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradComeDownLeftActions[] = {
  CR_ANIM_LOOK_LEFT, NULL, NULL,                                                // and looking left again
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradClimbUpLeftActions[] = {
  CR_ANIM_LOOK_LEFT, NULL, NULL,                                                // and looking left again  
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradClimbDownLeftActions[] = {
  CR_ANIM_JUST_HANGING_LEFT, NULL, NULL,                                        // just hanging... (for one frame)  
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradFallingLeftActions[] = {
  CR_ANIM_LOOK_LEFT, NULL, conradCanStand,                                      // and we hit the ground, ouch!
  CR_ANIM_FALLING_LEFT, NULL, NULL,                                             // must still be falling..  
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

action_map conradFallingRightActions[] = {
  CR_ANIM_LOOK_RIGHT, NULL, conradCanStand,                                     // and we hit the ground, ouch!
  CR_ANIM_FALLING_RIGHT, NULL, NULL,                                            // must still be falling..
  CR_END, NULL, NULL,                                                           // end (we shouldn't get here)
};

// now define our animations
// note that movements are multiples of 32
animation conradAnim[] = {
    0,    0,  false,   0.0,  0.0, conradLookLeftActions,                        //  0 - look left
    0,    6,  false,   0.0,  0.0, conradLeftToRightActions,                     //  1 - turn from looking left to looking right
    6,    6,  false,   0.0,  0.0, conradLookRightActions,                       //  2 - look right
    6,    0,  false,   0.0,  0.0, conradRightToLeftAction,                      //  3 - turn from looking right to looking left
    
   11,   15,  false,   1.0,  0.0, conradWalkRight1Actions,                      //  4 - walk right (left foot)
    7,   10,  false,   0.0,  0.0, conradWalkRight2Actions,                      //  5 - stop walking right (after left foot)
   16,   22,  false,   1.0,  0.0, conradWalkRightStopActions,                   //  6 - walk right (right foot)
   
   11,   15,   true,  -1.0,  0.0, conradWalkLeft1Actions,                       //  7 - walk left (left foot, uhm or right foot, mirrored you know) 
    7,   10,   true,  -0.0,  0.0, conradWalkLeft2Actions,                       //  8 - stop walking left (after left foot)
   16,   22,   true,  -1.0,  0.0, conradWalkLeftStopActions,                    //  9 - walk left (right foot)
   
   24,   31,   true,   0.0,  0.0, conradGetDownRightActions,                    // 10 - get down (facing right)
   31,   31,   true,   0.0,  0.0, conradStayDownRightActions,                   // 11 - stay down (facing right)
   32,   45,   true,   4.0,  0.0, conradRollRightActions,                       // 12 - roll (108 => 128)
   45,   32,   true,  -4.0,  0.0, conradRevRollRightActions,                    // 13 - reverse roll
   46,   46,   true,   1.0,  0.0, NULL,                                         // 14 - get up 1 (20 => 32)
   47,   49,   true,   0.0,  0.0, conradGetUp2RightActions,                     // 15 - get up 2

   24,   31,  false,   0.0,  0.0, conradGetDownLeftActions,                      // 16 - get down (facing left)
   31,   31,  false,   0.0,  0.0, conradStayDownLeftActions,                     // 17 - stay down (facing left)
   32,   45,  false,  -4.0,  0.0, conradRollLeftActions,                         // 18 - roll (-108 => -128)
   45,   32,  false,   4.0,  0.0, conradRevRollLeftActions,                      // 19 - reverse roll
   46,   46,  false,  -1.0,  0.0, NULL,                                          // 20 - get up 1 (-20 => -32)
   47,   49,  false,   0.0,  0.0, conradGetUp2LeftActions,                       // 21 - get up 2

   50,   66,   true,   0.0, -2.0, conradJumpUpRightActions,                      // 22 - jump up (facing right)
   80,   88,   true,   0.0,  0.0, conradJustHangRightActions,                    // 23 - just hanging (facing right)
   68,   78,   true,   0.0,  2.0, conradComeDownRightActions,                    // 24 - coming back down (facing right)
   89,  114,   true,   0.0, -3.0, conradClimbUpRightActions,                     // 25 - climbing up (facing right)
  114,   89,   true,   0.0,  3.0, conradClimbDownRightActions,                   // 26 - climbing down (facing right) 

   50,   66,  false,   0.0, -2.0, conradJumpUpLeftActions,                       // 27 - jump up (facing left)
   80,   88,  false,   0.0,  0.0, conradJustHangLeftActions,                     // 28 - just hanging (facing left)
   68,   78,  false,   0.0,  2.0, conradComeDownLeftActions,                     // 29 - coming back down (facing left)   
   89,  114,  false,   0.0, -3.0, conradClimbUpLeftActions,                      // 30 - climbing up (facing left)
  114,   89,  false,   0.0,  3.0, conradClimbDownLeftActions,                    // 31 - climbing down (facing left)
  
    0,    0,  false,   0.0,  1.0, conradFallingLeftActions,                      // 32 - falling left (needs different anim)
    6,    6,  false,   0.0,  1.0, conradFallingRightActions,                     // 33 - falling right (needs different anim)
  
};
