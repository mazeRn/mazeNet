#ifndef MAZENET_CLIENT_MOVE_H
#define MAZENET_CLIENT_MOVE_H

#include "Card.hpp"
#include "Position.hpp"

struct Move {
  Move();

  Card shift_card;
  Position shift_pos;
  Position new_pos;
  int value = 0;

};


#endif //MAZENET_CLIENT_MOVE_H
