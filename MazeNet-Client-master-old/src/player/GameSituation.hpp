#ifndef MAZENET_CLIENT_GAMELOGIC_HPP
#define MAZENET_CLIENT_GAMELOGIC_HPP


#include "Board.hpp"
#include "Player.hpp"
#include "Move.hpp"
#include <set>

struct GameSituation {
  void perform_shift(Move m);

  void setPlayerPos(Position new_pos);

  Board board_ = Board();
  Card shiftCard_ = Card();
  int forbidden_row_ = -1;
  int forbidden_col_ = -1;
  Player players_[4];
  int treasure_ = -1;
  int player_count_ = -1;
  std::set<int> found_treasures_ = std::set<int>{};
  int player_id_ = -1;
};


#endif //MAZENET_CLIENT_GAMELOGIC_HPP
