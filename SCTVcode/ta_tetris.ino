// Tetris game code

// for collision checking
#define ROTATE 1
#define LEFT   2
#define RIGHT  3
#define FALL   4

// bucket offset on the panel
#define BUCKET_OFFS_X 0
#define BUCKET_OFFS_Y 0

int bucket[20];

unsigned int last_interaction =    0; // when was the last button pressed?
unsigned int next_tick        =    0; // when will the next automatic step happen?
int          tick_length      =  500; // set in reset_tetris()  how long does on step take? (decreases on higher levels)

// scores
int level;
int lines;
int points;

// current tetromino
int tetr_type, next_tetr_type;
int tetr_rotation;
int tetr_col;
int tetr_row;

bool game_over   = false;
bool key_pressed = false;

/**
 * those are the 7 different tetris bricks, each placed in a 4x4 square
 * set bits are the parts where a brick is solid
 * the four quartets represent the four directions a tetromino can rotate
 */
const int TETROMINOES[7][4] = {
  // X
  // X
  // X
  // X
  {0b0000000011110000, 0b0100010001000100, 0b0000000011110000, 0b0100010001000100},

  // XX
  // XX
  {0b0000000001100110, 0b0000000001100110, 0b0000000001100110, 0b0000000001100110},

  // XX
  //  XX
  {0b0000000001100011, 0b0000001001100100, 0b0000000001100011, 0b0000001001100100},

  //  XX
  // XX
  {0b0000000001101100, 0b0000100011000100, 0b0000000001101100, 0b0000100011000100},

  // X
  // X
  // XX
  {0b0000000001110100, 0b0000010001000110, 0b0000000000010111, 0b0000011000100010},

  //  X
  //  X
  // XX
  {0b0000000011100010, 0b0000011001000100, 0b0000000010001110, 0b0000001000100110},

  //  X
  // XXX
  {0b0000000011100100, 0b0000010001100100, 0b0000010011100000, 0b0000010011000100}
};

int positionValue = 0;
int rotationValue = 0;

// Drawing the bucket is done in two dimensions.
// The goal is to show an outline of all occupied locations.
// The bucket is scanned in each axis, looking for borders.
// Each border line segment is drawn, taking into account the neighbors.

// First, the horizontal bars are drawn, based on the borders in the X axis.
// The bucket is scanned horizontally, row by row.
// A line is drawn at the bottom of a row if the row has 1s and the row below has 0s.
// A line is drawn at the top of the row below if the row has 0s and the row below has 1s.
// The start of the line depends on the row contents to the left:
//   00 is left of this col
//   11 is right of prev col.

// horiz line starting offsets for all combinations of prev and it
const int hsxArray[4][4] = {{0,  bo,  bo, 0}, {0, 0,  bo, 0}, {0,  bo, 0, 0}, {0, -bo, -bo, 0}};
const int hsyArray[4][4] = {{0, -bo,  bo, 0}, {0, 0,  bo, 0}, {0, -bo, 0, 0}, {0, -bo,  bo, 0}};

// horiz line ending offsets for all combinations of prev and it
const int hexArray[4][4] = {{0, 0, 0, 0}, {-bo, 0, -bo,  bo}, {-bo, -bo, 0, bo}, {0, 0, 0, 0}};
const int heyArray[4][4] = {{0, 0, 0, 0}, {-bo, 0, -bo, -bo}, { bo,  bo, 0, bo}, {0, 0, 0, 0}};

// vert line starting offsets for all combinations of prev and it
const int vsxArray[4][4] = {{0, -bo,  bo, 0}, {0, 0,  bo, 0}, {0, -bo, 0, 0}, {0, -bo,  bo, 0}};
const int vsyArray[4][4] = {{0, -bo, -bo, 0}, {0, 0, -bo, 0}, {0, -bo, 0, 0}, {0,  bo,  bo, 0}};

// vert line ending offsets for all combinations of prev and it
const int vexArray[4][4] = {{0, 0, 0, 0}, {-bo, 0, -bo, -bo}, { bo,  bo, 0,  bo}, {0, 0, 0, 0}};
const int veyArray[4][4] = {{0, 0, 0, 0}, { bo, 0,  bo, -bo}, { bo, -bo, 0, -bo}, {0, 0, 0, 0}};

int bsx, bsy, bex, bey = 0;

void DrawBucketHorizBars(void)
{
  int it, prev = 0;        // the bucket status (0..3) at col,row and col-1, row

  // scan all rows in the bucket, finding any horizontal border lines and drawing them
  for (int row = 0; row<18; row++)
  {
    prev = 0;    // col -1 is always empty
    //   if (frame%50 == 0) Serial.printf("row %2d  bucket %04X ", row, bucket[row]);
    for (int col = 0; col < 14; col++) // do one extra column to get the last position displayed
    {
      // current bucket bit, encoded with row below into 2 bits to find borders
      it = bitRead(bucket[row], col)*2 + bitRead(bucket[row+1], col);

      // a line ends in this case - draw it
      if ((prev != it) && ((prev == 1) || (prev == 2)))   // different, border between rows, prev on
      {
        bex = left_pos + col*tx + hexArray[prev][it];
        bey = top_pos  - (row+1)*ty + heyArray[prev][it];
        drawALine(bsx, bsy, bex, bey);
      }

      // a line starts in this case
      if ((prev != it) && ((it == 1) || (it == 2)))   // different, border between rows, it on
      {
        bsx = left_pos + col*tx + hsxArray[prev][it];
        bsy = top_pos  - (row+1)*ty + hsyArray[prev][it];
      }
      prev = it;
    }
 //   if (frame%50 == 0) Serial.println(" ");
  }
}

void DrawBucketVertBars(void)
{
  int it, prev = 0;        // the bucket status (0..3) at col,row and col, row-1

  // scan all columns in the bucket, finding any vertical border lines and drawing them
  for (int col = 1; col < 14; col++) // do one extra column to get the last position displayed
  {
 //   if (frame%50 == 0) Serial.printf("\n");
    prev = 0;    // row 0 is always empty
    for (int row = 0; row<18; row++)
    {
      // current bucket bit, encoded with col to left into 2 bits to find borders
      it = bitRead(bucket[row], col)*2 + bitRead(bucket[row], col-1);
//      if (frame%50 == 0) Serial.printf("%d ", it);

      // a line ends in this case - draw it
      if ((prev != it) && ((prev == 1) || (prev == 2)))   // different, border between rows, prev on
      {
        bex = left_pos + col*tx + vexArray[prev][it];
        bey = top_pos  - row*ty + veyArray[prev][it];
        drawALine(bsx, bsy, bex, bey);
      }

      // a line starts in this case
      if ((prev != it) && ((it == 1) || (it == 2)))   // different, border between rows, it on
      {
        bsx = left_pos + col*tx + vsxArray[prev][it];
        bsy = top_pos  - row*ty + vsyArray[prev][it];
      }
      prev = it;
    }
 //   if (frame%50 == 0) Serial.println(" ");
  }
}

// draw the things needed for Tetris. This will eventually have more stuff in it.
void drawTetris(void)
{
  Scale = 1;    // make them fit!
  int leftPaddle = 0;
  int rightPaddle = 0;
  for (int i=0; i<40; i++) {
    leftPaddle  += analogRead(XPosPin);   // read the position controls
    rightPaddle += analogRead(YPosPin);   // make them bipolar so midpoint is nominal
  }

  // center then in the display area, and scale for the right amount of overshoot
  // rotation appears to be backwards, so it gets negated
  // 0..3 is its range, give 3 sets of 4 over knob range
  rotationValue  = ((40960-rightPaddle)%18000) / 4500 ;

  positionValue = leftPaddle / 4096;     // 0..9 is its range
  if (positionValue > 9) positionValue = 9;
  if (positionValue < 0) positionValue = 0;

  DispBlock(next_tetr_type, 0, 16, 5);
  DispBlock(tetr_type, tetr_rotation, tetr_col + 1, tetr_row);
  DrawBucketHorizBars();
  DrawBucketVertBars();
}

// Tetris draw list just does the score. Court is drawn by drawTetris().
char scoreStr[] = "00000";
struct item tetrisList[] = {
  {ItemType::text,10,0,scoreStr,800, 0},
  {ItemType::listend,0,0,BlankLn,0,0}
};
