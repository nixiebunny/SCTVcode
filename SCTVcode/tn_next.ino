/**
 * a tetromino has reached the floor
 * calculate scores, check for completed rows etc.
 */
void handle_next() {
  add_to_bucket();

  // are there completed rows?
  check_rows();

  throw_next();
}

/** 
 * add the current tetromino to the bucket 
 */
void add_to_bucket() {
  int tet = TETROMINOES[tetr_type][tetr_rotation];

  for ( int i = 0; i < 16; i++ ) {
    if ( bitRead(tet, i) ) {
      int row = floor(i / 4);
      int col = i % 4;

      bitSet(bucket[row+tetr_row], col+tetr_col+1);
    }
  }

  points += 4 * level;
}

/**
 * and throw in the next tetromino
 */
void throw_next() {
  tetr_type  = next_tetr_type;

  get_next_tetromino();

  drop_tetromino();

  if ( check_collision() ) {
    game_over = true;

  }
}

/**
 * check for completed rows to remove
 */
void check_rows() {
  int completed = 0;

  for ( int i = 0; i < 17; i++ ) {
    if ( bucket[i] == 0b1111111111110) {
      completed++;

      // only the next 3 rows could also be effected
      if ( bucket[i+1] == 0b1111111111110) {
        completed++;
      }
      if ( bucket[i+2] == 0b1111111111110) {
        completed++;
      }
      if ( bucket[i+3] == 0b1111111111110) {
        completed++;
      }

      // shift the bucket 
      bucket[0] = 0b1000000000010;   // give it walls to feed on
      for ( int y = i + completed - 1; y >= 0; y-- ) {
        if ( y - completed < 0 )
          bucket[y] = 0b1000000000010;
        else
          bucket[y] = bucket[y - completed];
      }
      bucket[0] = 0;               // clear the top empty row

      break;
    }
  }

  if ( completed == 0 )
    return;

  delay(10);

  // scores
  switch( completed ) {
    case 1:
      points += (40 * level);
      break;

    case 2:
      points += (100 * level);
      break;

    case 3:
      points += (300 * level);
      break;

    case 4:
      points += (800 * level);
      break;
  }

  if (floor(lines/5) < floor((lines + completed)/5) ) {
    level++;
    tick_length = tick_length * 9 / 10;
  }

  // count rows
  lines += completed;
}

/**
 * handle key presses for movement etc.
 */
void do_tetris_keys() {
  
 // Serial.printf("prev %d this %d tetr_col %d\n", prevPosition, positionValue, tetr_col);
  
  if (positionValue > tetr_col+1) {
    move_tetromino_right();
  }
  if (positionValue < tetr_col+1) {
    move_tetromino_left();
  }
  tetr_rotation = rotationValue;
  
  sprintf (scoreStr, "%5d", points);
}

/**
 * start a new game
 */
void reset_tetris() {
  // get the randomizer running
  unsigned long seed = 0;
  
  for ( int i = 0; i < 32; i++ ) {
    seed = seed | ((analogRead(randPin) & 0x01) << i);
  }

  randomSeed ( seed );

  tick_length = 600;
  
  // note: the top of the bucket corresponds to the floor on the panel
  // so it's upside down
  bucket[0] = bucket[17] = 0;
  for ( int i = 1; i < 17; i++ )
    bucket[i] = 0b1000000000010;

  // reset scores
  level  = 8;
  lines  = 0;
  points = 0;

  next_tick = millis() + tick_length;

  game_over = false;   // start a new game!
}
