#ifndef MAZENET_CLIENT_HEURISTICSTRATEGY_HPP
#define MAZENET_CLIENT_HEURISTICSTRATEGY_HPP

#include "IPlayerStrategy.hpp"
#include "Move.hpp"
#include "HeuristicSettings.hpp"

#include <vector>

class HeuristicStrategy : public IPlayerStrategy {
 public:
  HeuristicStrategy();

  virtual Move calculate_next_move() override;

  virtual void move_accepted() override;

  virtual void move_rejected() override;

 private:

  HeuristicSettings settings_;

  int evaluate_base_score(const GameSituation& situation, const std::vector<Position>& possiblePositions_);

  int evaluate_position_score(const GameSituation& situation, const Position& position, int& situations);

  int evaluate_enemy_score(const GameSituation& situation, int& situations);
};


#endif //MAZENET_CLIENT_HEURISTICSTRATEGY_HPP
