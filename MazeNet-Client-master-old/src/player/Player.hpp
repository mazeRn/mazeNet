#ifndef MAZENET_CLIENT_PLAYER_HPP
#define MAZENET_CLIENT_PLAYER_HPP


#include "Position.hpp"

struct Player {
  int remainingTreasures_ = 0;
  Position home_ = {-1, -1};
  Position pos_ = {-1, -1};
};


#endif //MAZENET_CLIENT_PLAYER_HPP
