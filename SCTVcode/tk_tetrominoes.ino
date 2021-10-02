/**
 * randomly get a new tetromino
 */
void get_next_tetromino() {
  // of course not every tetromino has the same probability being the next one
  switch ( random(17) ) {
    case 0:
      next_tetr_type = 0;
      break;

    case 1:
    case 2:
    case 3:
      next_tetr_type = 1;
      break;

    case 4:
    case 5:
    case 6:
      next_tetr_type = 2;
      break;

    case 7:
    case 8:
    case 9:
      next_tetr_type = 3;
      break;

    case 10:
    case 11:
    case 12:
      next_tetr_type = 4;
      break;

    case 13:
    case 14:
    case 15:
      next_tetr_type = 5;
      break;

    default:
      next_tetr_type = 6;
      break;
  }
}

/**
 * throws a tetromino in the bucket
 */
void drop_tetromino() {
  // reset positions
  tetr_col = 4;
  tetr_row = 1;

  tetr_rotation = 0;
}

/**
 * tetromino movements
 */

bool rotate_tetromino() {
  if ( check_collision(ROTATE) )
    return false;

  tetr_rotation++;
  tetr_rotation %= 4;

  return true;
}

bool move_tetromino_left() {
  if ( check_collision(LEFT) )
    return false;

  tetr_col--;

  return true;
}

bool move_tetromino_right() {
  if ( check_collision(RIGHT) )
    return false;

  tetr_col++;

  return true;
}

bool move_tetromino_down() {
  if ( check_collision(FALL) ) {
    handle_next();
    return false;
  }

  tetr_row++;

  return true;
}
