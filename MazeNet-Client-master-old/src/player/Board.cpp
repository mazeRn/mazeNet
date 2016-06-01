#include "Board.hpp"

Card Board::insert_card(Card card, Position new_pos) {
  // Card is inserted on right side
  Card save;
  if (new_pos.col == 6) {
    // save left card of current row
    save = cards_[new_pos.row][0];

    // shift row to the left
    for (int col = 0; col < 6; ++col) {
      cards_[new_pos.row][col] = cards_[new_pos.row][col + 1];
    }

    // set new card
    cards_[new_pos.row][new_pos.col] = card;
    // return new shift card
    return save;
  }
    // Card is inserted on left side
  else if (new_pos.col == 0) {
    // save right card of current row
    save = cards_[new_pos.row][6];

    // shift row to the right
    for (int col = 6; col > 0; --col) {
      cards_[new_pos.row][col] = cards_[new_pos.row][col - 1];
    }

    // set new card
    cards_[new_pos.row][new_pos.col] = card;
    // return new shift card
    return save;
  }
    // card is inserted on top side
  else if (new_pos.row == 0) {
    // save bottom card of column
    save = cards_[6][new_pos.col];

    // shift column down
    for (int row = 6; row > 0; --row) {
      cards_[row][new_pos.col] = cards_[row - 1][new_pos.col];
    }

    // set new card
    cards_[new_pos.row][new_pos.col] = card;
    // return new shift card
    return save;
  }
    // card is inserted on bottom side
  else if (new_pos.row == 6) {
    // save top card of column
    save = cards_[0][new_pos.col];

    // shift column up
    for (int row = 0; row < 6; ++row) {
      cards_[row][new_pos.col] = cards_[row + 1][new_pos.col];
    }
    cards_[new_pos.row][new_pos.col] = card;
  }

  cards_[new_pos.row][new_pos.col].pins_ = save.pins_;
  save.pins_ = 0;

  return save;
}