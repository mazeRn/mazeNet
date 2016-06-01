#ifndef MAZENET_CLIENT_IPLAYERSTRATEGY_HPP
#define MAZENET_CLIENT_IPLAYERSTRATEGY_HPP


#include "GameSituation.hpp"
#include "Move.hpp"

class IPlayerStrategy {
 public:
  virtual ~IPlayerStrategy();

  virtual Move calculate_next_move() = 0;

  virtual void move_accepted() = 0;
  virtual void move_rejected() = 0;

  GameSituation situation_ = GameSituation();
};


#endif //MAZENET_CLIENT_IPLAYERSTRATEGY_HPP
