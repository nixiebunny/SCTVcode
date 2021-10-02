// --------------------- Tetris objects ---------------------------

// The coordinate system is based on the game that used an LED matrix. 
// Top left is (0,0) and scale of the objects matches this screen's draw space of 2500 square. 
// If not, it's all parameterized below for ease of changes. 
// The bucket is an array of bits, each word is one row of the bucket. 
// The col (bit) number is 1 left wall, 2..11 contents, 12 right wall. 
// bits 0 and 13 are outside borders so that the drawing edge detector can see the bucket walls.
// The array index is 0 empty top row, 17 bottom. 

const int top_pos = 1000;
const int left_pos = -800;

const int bo = 6;    // border inset from edge of char
const int tx = 100;   // width of a cell
const int ty = 100;   // height of a cell

// These are the tetronimo draw lists in order that the code identifies them. 
// We don't need to define the duplicates, they're used by reference in the lookup table.
const int tetronI02[]   = 
{ 
  lin, 0*tx+bo,  0*ty-bo,  4*tx-bo,  0*ty-bo, 0,0, // horizontal bar 1 below
  lin, 4*tx-bo,  0*ty-bo,  4*tx-bo, -1*ty+bo, 0,0, 
  lin, 4*tx-bo, -1*ty+bo,  0*tx+bo, -1*ty+bo, 0,0, 
  lin, 0*tx+bo, -1*ty+bo,  0*tx+bo,  0*ty-bo, 0,0, 0x80
};

const int tetronI13[]   = 
{ lin, 2*tx+bo,  1*ty-bo,  3*tx-bo,  1*ty-bo, 0,0, // vertical bar 2 right
  lin, 3*tx-bo,  1*ty-bo,  3*tx-bo, -3*ty+bo, 0,0, 
  lin, 3*tx-bo, -3*ty+bo,  2*tx+bo, -3*ty+bo, 0,0, 
  lin, 2*tx+bo, -3*ty+bo,  2*tx+bo,  1*ty-bo, 0,0, 0x80
};

const int tetronO[]     = 
{ lin, 1*tx+bo,  1*ty-bo,  3*tx-bo,  1*ty-bo, 0,0, // square top middle
  lin, 3*tx-bo,  1*ty-bo,  3*tx-bo, -1*ty+bo, 0,0, 
  lin, 3*tx-bo, -1*ty+bo,  1*tx+bo, -1*ty+bo, 0,0, 
  lin, 1*tx+bo, -1*ty+bo,  1*tx+bo,  1*ty-bo, 0,0, 0x80
};

const int tetronZ02[]   = 
{ lin, 0*tx+bo,  1*ty-bo,  2*tx-bo,  1*ty-bo, 0,0, // z horizontal
  lin, 2*tx-bo,  1*ty-bo,  2*tx-bo,  0*ty-bo, 0,0, 
  lin, 2*tx-bo,  0*ty-bo,  3*tx-bo,  0*ty-bo, 0,0, 
  lin, 3*tx-bo,  0*ty-bo,  3*tx-bo, -1*ty+bo, 0,0, 
  lin, 3*tx-bo, -1*ty+bo,  1*tx+bo, -1*ty+bo, 0,0, 
  lin, 1*tx+bo, -1*ty+bo,  1*tx+bo,  0*ty+bo, 0,0,
  lin, 1*tx+bo,  0*ty+bo,  0*tx+bo,  0*ty+bo, 0,0, 
  lin, 0*tx+bo,  0*ty+bo,  0*tx+bo,  1*ty-bo, 0,0, 0x80
};

const int tetronZ13[]   = 
{ lin, 2*tx+bo,  1*ty-bo,  3*tx-bo,  1*ty-bo, 0,0, // z vertical
  lin, 3*tx-bo,  1*ty-bo,  3*tx-bo, -1*ty+bo, 0,0, 
  lin, 3*tx-bo, -1*ty+bo,  2*tx-bo, -1*ty+bo, 0,0, 
  lin, 2*tx-bo, -1*ty+bo,  2*tx-bo, -2*ty+bo, 0,0, 
  lin, 2*tx-bo, -2*ty+bo,  1*tx+bo, -2*ty+bo, 0,0, 
  lin, 1*tx+bo, -2*ty+bo,  1*tx+bo,  0*ty-bo, 0,0,
  lin, 1*tx+bo,  0*ty-bo,  2*tx+bo,  0*ty-bo, 0,0, 
  lin, 2*tx+bo,  0*ty-bo,  2*tx+bo,  1*ty-bo, 0,0, 0x80
};

const int tetronS02[]   = 
{ lin, 2*tx+bo,  1*ty-bo,  4*tx-bo,  1*ty-bo, 0,0, // S horizontal
  lin, 4*tx-bo,  1*ty-bo,  4*tx-bo,  0*ty+bo, 0,0, 
  lin, 4*tx-bo,  0*ty+bo,  3*tx-bo,  0*ty+bo, 0,0, 
  lin, 3*tx-bo,  0*ty+bo,  3*tx-bo, -1*ty+bo, 0,0, 
  lin, 3*tx-bo, -1*ty+bo,  1*tx+bo, -1*ty+bo, 0,0, 
  lin, 1*tx+bo, -1*ty+bo,  1*tx+bo,  0*ty-bo, 0,0,
  lin, 1*tx+bo,  0*ty-bo,  2*tx+bo,  0*ty-bo, 0,0, 
  lin, 2*tx+bo,  0*ty-bo,  2*tx+bo,  1*ty-bo, 0,0, 0x80
};

const int tetronS13[]   = 
{ lin, 2*tx+bo,  1*ty-bo,  3*tx-bo,  1*ty-bo, 0,0, // S vertical
  lin, 3*tx-bo,  1*ty-bo,  3*tx-bo,  0*ty-bo, 0,0, 
  lin, 3*tx-bo,  0*ty-bo,  4*tx-bo,  0*ty-bo, 0,0, 
  lin, 4*tx-bo,  0*ty-bo,  4*tx-bo, -2*ty+bo, 0,0, 
  lin, 4*tx-bo, -2*ty+bo,  3*tx+bo, -2*ty+bo, 0,0, 
  lin, 3*tx+bo, -2*ty+bo,  3*tx+bo, -1*ty+bo, 0,0,
  lin, 3*tx+bo, -1*ty+bo,  2*tx+bo, -1*ty+bo, 0,0, 
  lin, 2*tx+bo, -1*ty+bo,  2*tx+bo,  1*ty-bo, 0,0, 0x80
};

const int tetronL0[]    = 
{ lin, 2*tx+bo,  1*ty-bo,  3*tx-bo,  1*ty-bo, 0,0, // L pointing left
  lin, 3*tx-bo,  1*ty-bo,  3*tx-bo, -1*ty+bo, 0,0, 
  lin, 3*tx-bo, -1*ty+bo,  0*tx+bo, -1*ty+bo, 0,0, 
  lin, 0*tx+bo, -1*ty+bo,  0*tx+bo,  0*ty-bo, 0,0,
  lin, 0*tx+bo,  0*ty-bo,  2*tx+bo,  0*ty-bo, 0,0, 
  lin, 2*tx+bo,  0*ty-bo,  2*tx+bo,  1*ty-bo, 0,0, 0x80
};

const int tetronL1[]    = 
{ lin, 1*tx+bo,  1*ty-bo,  3*tx-bo,  1*ty-bo, 0,0, // L pointing down
  lin, 3*tx-bo,  1*ty-bo,  3*tx-bo, -2*ty+bo, 0,0, 
  lin, 3*tx-bo, -2*ty+bo,  2*tx+bo, -2*ty+bo, 0,0, 
  lin, 2*tx+bo, -2*ty+bo,  2*tx+bo,  0*ty+bo, 0,0,
  lin, 2*tx+bo,  0*ty+bo,  1*tx+bo,  0*ty+bo, 0,0, 
  lin, 1*tx+bo,  0*ty+bo,  1*tx+bo,  1*ty-bo, 0,0, 0x80
};

const int tetronL2[]    = 
{ lin, 0*tx+bo,  1*ty-bo,  3*tx-bo,  1*ty-bo, 0,0, // L pointing right
  lin, 3*tx-bo,  1*ty-bo,  3*tx-bo,  0*ty+bo, 0,0, 
  lin, 3*tx-bo,  0*ty+bo,  1*tx-bo,  0*ty+bo, 0,0, 
  lin, 1*tx-bo,  0*ty+bo,  1*tx-bo, -1*ty+bo, 0,0,
  lin, 1*tx-bo, -1*ty+bo,  0*tx+bo, -1*ty+bo, 0,0, 
  lin, 0*tx+bo, -1*ty+bo,  0*tx+bo,  1*ty-bo, 0,0, 0x80
};

const int tetronL3[]    = 
{ lin, 1*tx+bo,  1*ty-bo,  2*tx-bo,  1*ty-bo, 0,0, // L pointing up
  lin, 2*tx-bo,  1*ty-bo,  2*tx-bo, -1*ty-bo, 0,0, 
  lin, 2*tx-bo, -1*ty-bo,  3*tx-bo, -1*ty-bo, 0,0, 
  lin, 3*tx-bo, -1*ty-bo,  3*tx-bo, -2*ty+bo, 0,0,
  lin, 3*tx-bo, -2*ty+bo,  1*tx+bo, -2*ty+bo, 0,0, 
  lin, 1*tx+bo, -2*ty+bo,  1*tx+bo,  1*ty-bo, 0,0, 0x80
};

const int tetronG0[]    = 
{ lin, 1*tx+bo,  1*ty-bo,  2*tx-bo,  1*ty-bo, 0,0, // backwards L pointing right
  lin, 2*tx-bo,  1*ty-bo,  2*tx-bo,  0*ty-bo, 0,0, 
  lin, 2*tx-bo,  0*ty-bo,  4*tx-bo,  0*ty-bo, 0,0, 
  lin, 4*tx-bo,  0*ty-bo,  4*tx-bo, -1*ty+bo, 0,0,
  lin, 4*tx-bo, -1*ty+bo,  1*tx+bo, -1*ty+bo, 0,0, 
  lin, 1*tx+bo, -1*ty+bo,  1*tx+bo,  1*ty-bo, 0,0, 0x80
};

const int tetronG1[]    = 
{ lin, 2*tx+bo,  1*ty-bo,  3*tx-bo,  1*ty-bo, 0,0, // backwards L pointing up
  lin, 3*tx-bo,  1*ty-bo,  3*tx-bo, -2*ty+bo, 0,0, 
  lin, 3*tx-bo, -2*ty+bo,  1*tx+bo, -2*ty+bo, 0,0, 
  lin, 1*tx+bo, -2*ty+bo,  1*tx+bo, -1*ty-bo, 0,0,
  lin, 1*tx+bo, -1*ty-bo,  2*tx+bo, -1*ty-bo, 0,0, 
  lin, 2*tx+bo, -1*ty-bo,  2*tx+bo,  1*ty-bo, 0,0, 0x80
};

const int tetronG2[]    = 
{ lin, 1*tx+bo,  1*ty-bo,  4*tx-bo,  1*ty-bo, 0,0, // backwards L pointing left
  lin, 4*tx-bo,  1*ty-bo,  4*tx-bo, -1*ty+bo, 0,0, 
  lin, 4*tx-bo, -1*ty+bo,  3*tx+bo, -1*ty+bo, 0,0, 
  lin, 3*tx+bo, -1*ty+bo,  3*tx+bo,  0*ty+bo, 0,0,
  lin, 3*tx+bo,  0*ty+bo,  1*tx+bo,  0*ty+bo, 0,0, 
  lin, 1*tx+bo,  0*ty+bo,  1*tx+bo,  1*ty-bo, 0,0, 0x80
};

const int tetronG3[]    = 
{ lin, 1*tx+bo,  1*ty-bo,  3*tx-bo,  1*ty-bo, 0,0, // backwards L pointing down
  lin, 3*tx-bo,  1*ty-bo,  3*tx-bo,  0*ty+bo, 0,0, 
  lin, 3*tx-bo,  0*ty+bo,  2*tx-bo,  0*ty+bo, 0,0, 
  lin, 2*tx-bo,  0*ty+bo,  2*tx-bo, -2*ty+bo, 0,0,
  lin, 2*tx-bo, -2*ty+bo,  1*tx+bo, -2*ty+bo, 0,0, 
  lin, 1*tx+bo, -2*ty+bo,  1*tx+bo,  1*ty-bo, 0,0, 0x80
};

const int tetronT0[]    = 
{ lin, 2*tx+bo,  1*ty-bo,  3*tx-bo,  1*ty-bo, 0,0, // T pointing up
  lin, 3*tx-bo,  1*ty-bo,  3*tx-bo,  0*ty-bo, 0,0, 
  lin, 3*tx-bo,  0*ty-bo,  4*tx-bo,  0*ty-bo, 0,0, 
  lin, 4*tx-bo,  0*ty-bo,  4*tx-bo, -1*ty+bo, 0,0, 
  lin, 4*tx-bo, -1*ty+bo,  1*tx+bo, -1*ty+bo, 0,0, 
  lin, 1*tx+bo, -1*ty+bo,  1*tx+bo,  0*ty-bo, 0,0,
  lin, 1*tx+bo,  0*ty-bo,  2*tx+bo,  0*ty-bo, 0,0, 
  lin, 2*tx+bo,  0*ty-bo,  2*tx+bo,  1*ty-bo, 0,0, 0x80
};

const int tetronT1[]    = 
{ lin, 2*tx+bo,  1*ty-bo,  3*tx-bo,  1*ty-bo, 0,0, // T pointing left
  lin, 3*tx-bo,  1*ty-bo,  3*tx-bo, -2*ty+bo, 0,0, 
  lin, 3*tx-bo, -2*ty+bo,  2*tx+bo, -2*ty+bo, 0,0, 
  lin, 2*tx+bo, -2*ty+bo,  2*tx+bo, -1*ty+bo, 0,0,
  lin, 2*tx+bo, -1*ty+bo,  1*tx+bo, -1*ty+bo, 0,0, 
  lin, 1*tx+bo, -1*ty+bo,  1*tx+bo,  0*ty-bo, 0,0,
  lin, 1*tx+bo,  0*ty-bo,  2*tx+bo,  0*ty-bo, 0,0, 
  lin, 2*tx+bo,  0*ty-bo,  2*tx+bo,  1*ty-bo, 0,0, 0x80
};

const int tetronT2[] = 
{ 
  lin, 1*tx+bo,  0*ty-bo,  4*tx-bo,  0*ty-bo, 0,0, // T pointing down
  lin, 4*tx-bo,  0*ty-bo,  4*tx-bo, -1*ty+bo, 0,0, 
  lin, 4*tx-bo, -1*ty+bo,  3*tx-bo, -1*ty+bo, 0,0, 
  lin, 3*tx-bo, -1*ty+bo,  3*tx-bo, -2*ty+bo, 0,0, 
  lin, 3*tx-bo, -2*ty+bo,  2*tx+bo, -2*ty+bo, 0,0, 
  lin, 2*tx+bo, -2*ty+bo,  2*tx+bo, -1*ty+bo, 0,0,
  lin, 2*tx+bo, -1*ty+bo,  1*tx+bo, -1*ty+bo, 0,0, 
  lin, 1*tx+bo, -1*ty+bo,  1*tx+bo,  0*ty-bo, 0,0, 0x80
};

const int tetronT3[] = 
{ 
  lin, 2*tx+bo,  1*ty-bo,  3*tx-bo,  1*ty-bo, 0,0, // T pointing right
  lin, 3*tx-bo,  1*ty-bo,  3*tx-bo,  0*ty-bo, 0,0, 
  lin, 3*tx-bo,  0*ty-bo,  4*tx-bo,  0*ty-bo, 0,0, 
  lin, 4*tx-bo,  0*ty-bo,  4*tx-bo, -1*ty+bo, 0,0, 
  lin, 4*tx-bo, -1*ty+bo,  3*tx-bo, -1*ty+bo, 0,0, 
  lin, 3*tx-bo, -1*ty+bo,  3*tx-bo, -2*ty+bo, 0,0,
  lin, 3*tx-bo, -2*ty+bo,  2*tx+bo, -2*ty+bo, 0,0,
  lin, 2*tx+bo, -2*ty+bo,  2*tx+bo,  1*ty-bo, 0,0, 0x80
};


// an array of pointers to the draw lists above
// sorted by [type][rot]
const int * tetronimoList[7][4] = 
{
  {tetronI02, tetronI13, tetronI02, tetronI13}, 
  {tetronO,   tetronO,   tetronO,   tetronO},    
  {tetronZ02, tetronZ13, tetronZ02, tetronZ13}, 
  {tetronS02, tetronS13, tetronS02, tetronS13}, 
  {tetronL0,  tetronL1,  tetronL2,  tetronL3},  
  {tetronG0,  tetronG1,  tetronG2,  tetronG3},  
  {tetronT0,  tetronT1,  tetronT2,  tetronT3}
}; 

// DispBlock displays one block of type with rotation rot 
// at col, row (upper left corner, units of blocks)
void DispBlock(int type, int rot, int col, int row) 
{
  int notLast;

  ChrXPos = col * tx + left_pos;  // transform coordinates from game to screen
  ChrYPos = top_pos - row * ty;
  TheSeg = tetronimoList[type][rot];   // look up what to draw
  notLast = GetSeg();   // get the first segment to draw
  while (notLast) 
  {
    DoSeg();           // display segments until last one found
    notLast = GetSeg();
  }
}
