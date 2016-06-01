#include "MoveCalculator.hpp"

std::vector<Move> MoveCalculator::get_possible_moves(const GameSituation& situation) {
  std::vector<Move> moves;
  int insert_col = 0;
  int insert_row = 0;

  for (insert_row = 1; insert_row < 7; insert_row += 2) {
    if (insert_col != situation.forbidden_col_ || insert_row != situation.forbidden_row_) {
      Card c = situation.shiftCard_;
      for (int i = 0; i < 4; ++i) {
        Move m;
        m.shift_card = c;
        m.shift_pos = {insert_row, insert_col};
        moves.emplace_back(m);
        c.rotateLeft();
      }
    }
  }

  insert_col = 6;
  for (insert_row = 1; insert_row < 7; insert_row += 2) {
    if (insert_col != situation.forbidden_col_ || insert_row != situation.forbidden_row_) {
      Card c = situation.shiftCard_;
      for (int i = 0; i < 4; ++i) {
        Move m;
        m.shift_card = c;
        m.shift_pos = {insert_row, insert_col};
        moves.emplace_back(m);
        c.rotateLeft();
      }
    }
  }

  insert_row = 0;
  for (insert_col = 1; insert_col < 7; insert_col += 2) {
    if (insert_col != situation.forbidden_col_ || insert_row != situation.forbidden_row_) {
      Card c = situation.shiftCard_;
      for (int i = 0; i < 4; ++i) {
        Move m;
        m.shift_card = c;
        m.shift_pos = {insert_row, insert_col};
        moves.emplace_back(m);
        c.rotateLeft();
      }
    }
  }

  insert_row = 6;
  for (insert_col = 1; insert_col < 7; insert_col += 2) {
    if (insert_col != situation.forbidden_col_ || insert_row != situation.forbidden_row_) {
      Card c = situation.shiftCard_;
      for (int i = 0; i < 4; ++i) {
        Move m;
        m.shift_card = c;
        m.shift_pos = {insert_row, insert_col};
        moves.emplace_back(m);
        c.rotateLeft();
      }
    }
  }

  return moves;
}

std::vector<Position> MoveCalculator::get_possible_positions(GameSituation& situation) {
  std::vector<Position> positions;
  bool visited[7][7];
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 7; ++j) {
      visited[i][j] = false;
    }
  }

  std::vector<Position> remaining;

  Position cur_pos = situation.players_[situation.player_id_ - 1].pos_;


  remaining.push_back(cur_pos);

  if (cur_pos.row == -1)
    return remaining;

  while (remaining.size() > 0) {
    cur_pos = remaining.back();
    visited[cur_pos.row][cur_pos.col] = true;
    remaining.pop_back();

    positions.push_back(cur_pos);

    // can move up
    if (cur_pos.row > 0) {
      if (situation.board_.cards_[cur_pos.row][cur_pos.col].isOpen(Card::UP) &&
          situation.board_.cards_[cur_pos.row - 1][cur_pos.col].isOpen(Card::DOWN) &&
          !visited[cur_pos.row - 1][cur_pos.col]) {
        Position p = {cur_pos.row - 1, cur_pos.col};
        remaining.emplace_back(p);
      }
    }
    // can move down
    if (cur_pos.row < 6) {
      if (situation.board_.cards_[cur_pos.row][cur_pos.col].isOpen(Card::DOWN) &&
          situation.board_.cards_[cur_pos.row + 1][cur_pos.col].isOpen(Card::UP) &&
          !visited[cur_pos.row + 1][cur_pos.col]) {
        Position p = {cur_pos.row + 1, cur_pos.col};
        remaining.emplace_back(p);
      }
    }

    // can move left
    if (cur_pos.col > 0) {
      if (situation.board_.cards_[cur_pos.row][cur_pos.col].isOpen(Card::LEFT) &&
          situation.board_.cards_[cur_pos.row][cur_pos.col - 1].isOpen(Card::RIGHT) &&
          !visited[cur_pos.row][cur_pos.col - 1]) {
        Position p = {cur_pos.row, cur_pos.col - 1};
        remaining.emplace_back(p);
      }
    }

    // can move right
    if (cur_pos.col < 6) {
      if (situation.board_.cards_[cur_pos.row][cur_pos.col].isOpen(Card::RIGHT) &&
          situation.board_.cards_[cur_pos.row][cur_pos.col + 1].isOpen(Card::LEFT) &&
          !visited[cur_pos.row][cur_pos.col + 1]) {
        Position p = {cur_pos.row, cur_pos.col + 1};
        remaining.emplace_back(p);
      }
    }
  }

  return positions;
}
