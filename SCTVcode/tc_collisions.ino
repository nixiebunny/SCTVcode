/**
 * collision detection
 * checks if a tetromino can move in a certain direction or rotate
 * returns true, if a collision is about to happen
 */
bool check_collision ( const int dir = 0 ) {
  int temp_tet = TETROMINOES[tetr_type][tetr_rotation];
  int temp_tetr_col = tetr_col;
  int temp_tetr_row = tetr_row;

  switch ( dir ) {
    case ROTATE:
      temp_tet = TETROMINOES[tetr_type][(tetr_rotation + 1)%4];
      break;

    case LEFT:
      temp_tetr_col--;
      break;

    case RIGHT:
      temp_tetr_col++;
      break;

    case FALL:
      temp_tetr_row++;
      break;

    default: // just dropped tetromino
      break;
  }

  // now check every set bit in the tetromino container
  // if it collides with a set bit in the bucket

  for ( int i = 0; i < 16; i++ ) {
    if ( bitRead(temp_tet, i) ) {
      int row = floor(i / 4);
      int col = i % 4;

      // the floor is not included in the bucket, so check it here
      if ( row+temp_tetr_row > 16 )
        return true;

      // temp_tetr_col is offset by another 1 because the walls of the bucket are included
      if ( bitRead(bucket[row+temp_tetr_row], col+temp_tetr_col+1) )
        return true;
    }
  }

  return false;
}
