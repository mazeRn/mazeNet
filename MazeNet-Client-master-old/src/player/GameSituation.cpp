#include "GameSituation.hpp"

void GameSituation::perform_shift(Move m) {
  shiftCard_ = board_.insert_card(m.shift_card, m.shift_pos);

  // aktualisiere Spielerpositionen
  if (m.shift_pos.row == 0 || m.shift_pos.row == 6) {
    for (int i = 0; i < 7; ++i) {
      for (int p = 1; p <= 4; ++p) {
        if (board_.cards_[i][m.shift_pos.col].hasPlayer(p)) {
          players_[p - 1].pos_ = {i, m.shift_pos.col};
        }
      }
    }
  }

  // aktualisiere Spielerpositionen
  if (m.shift_pos.col == 0 || m.shift_pos.col == 6) {
    for (int i = 0; i < 7; ++i) {
      for (int p = 1; p <= 4; ++p) {
        if (board_.cards_[m.shift_pos.row][i].hasPlayer(p)) {
          players_[p - 1].pos_ = {m.shift_pos.row, i};
        }
      }
    }
  }

  // Spieler verlässt Spielfeld
  for (int i = 1; i <= 4; ++i) {
    if (shiftCard_.hasPlayer(i)) {
      players_[i - 1].pos_ = {m.shift_pos.row, m.shift_pos.col};
    }
  }
}

void GameSituation::setPlayerPos(Position new_pos) {
  Position old_pos = players_[player_id_ - 1].pos_;
  if (old_pos.row != -1)
    board_.cards_[old_pos.row][old_pos.col].removePlayer(player_id_);

  players_[player_id_ - 1].pos_ = new_pos;
  if (new_pos.row != -1)
    board_.cards_[new_pos.row][new_pos.col].setPlayer(player_id_);
}
