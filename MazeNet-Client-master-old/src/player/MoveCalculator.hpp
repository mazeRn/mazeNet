#ifndef MAZENET_CLIENT_MOVECALCULATOR_HPP
#define MAZENET_CLIENT_MOVECALCULATOR_HPP


#include <vector>
#include "Move.hpp"
#include "GameSituation.hpp"

class MoveCalculator {
 public:
  static std::vector<Move> get_possible_moves(const GameSituation& situation);

  static std::vector<Position> get_possible_positions(GameSituation& situation);

 private:

};


#endif //MAZENET_CLIENT_MOVECALCULATOR_HPP
