#include <util/logging/Log.hpp>
#include "HeuristicStrategy.hpp"
#include "MoveCalculator.hpp"
#include <chrono>

HeuristicStrategy::HeuristicStrategy() : settings_() { }

Move HeuristicStrategy::calculate_next_move() {
  using namespace mazenet::util::logging;
  auto t1 = std::chrono::high_resolution_clock::now();

  Log logger("heuristic");
  Move best_move;
  int best_score = INT32_MIN;
  logger.logSeverity(SeverityLevel::info) << "Evaluating: \n" << situation_.board_
  << "\nShift Card: " << situation_.shiftCard_
  << "\nPlayer Pos: " << "R: " << situation_.players_[situation_.player_id_ - 1].pos_.row
  << " C: " << situation_.players_[situation_.player_id_ - 1].pos_.col << logger.end();

  std::vector<Move> available_basic_moves = MoveCalculator::get_possible_moves(situation_);

  // generiere mögliche Züge
  int situations = 0;

#pragma omp parallel
  {
    int local_best_score = INT32_MIN;
    Move local_best_move;

#pragma omp for reduction(+:situations)
    for (size_t i = 0; i < available_basic_moves.size(); ++i) {
      GameSituation cur_situation = situation_;
      cur_situation.perform_shift(available_basic_moves[i]);

      std::vector<Position> positions = MoveCalculator::get_possible_positions(cur_situation);
#if 0
      logger.logSeverity(SeverityLevel::debug) << "Choice: \n" << cur_situation.board_
      << "\nPositions: " << logger.end();
#endif
      int base_score = evaluate_base_score(cur_situation, positions);

      base_score -= evaluate_enemy_score(cur_situation, situations);

      Move current_move = available_basic_moves[i];
      for (auto pos: positions) {

        current_move.new_pos = pos;

        int actual_score = base_score;
        actual_score += evaluate_position_score(cur_situation, pos, situations);
#if 0
        logger.log() << "\nR: " << pos.row << " C: " << pos.col
        << " Score: " << actual_score << logger.end();
#endif

        if (actual_score > local_best_score) {
          local_best_move = current_move;
          local_best_score = actual_score;
        }
        situations++;
      }
    }

#pragma omp critical
    {
      if (local_best_score > best_score) {
        best_score = local_best_score;
        best_move = local_best_move;
      }
    }
  }
  auto t2 = std::chrono::high_resolution_clock::now();

  logger.logSeverity(SeverityLevel::notification) << "Calculated situations: " << situations
  << "\nTime: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count() << "ms" << logger.end();

  logger.logSeverity(SeverityLevel::notification) << "Best Move: (" << best_score << ")"
  << "\nCard: " << best_move.shift_card << " R: " << best_move.shift_pos.row << " C: " << best_move.shift_pos.col
  << "\nPlayer: " << " R: " << best_move.new_pos.row << " C: " << best_move.new_pos.col << logger.end();

  return best_move;
}

void HeuristicStrategy::move_accepted() {

}

void HeuristicStrategy::move_rejected() {

}

int HeuristicStrategy::evaluate_base_score(const GameSituation& situation,
                                           const std::vector<Position>& possiblePositions) {
  int score = 0;

  // base score for reachable fields
  score += possiblePositions.size() * settings_.reachableFieldValue;

  for (auto pos: possiblePositions) {
    auto& cur_card = situation.board_.cards_[pos.row][pos.col];
    // bonus for remaining reachable treasure on map
    if (cur_card.getTreasure() > 3 &&
        situation.found_treasures_.find(cur_card.getTreasure()) == situation.found_treasures_.end()) {
      score += settings_.reachableTreasureValue;
    }

    // bonus for reachable home, scaled by remaining treasures, since it will gradually become more important
    int remaining = situation.players_[situation.player_id_ - 1].remainingTreasures_;
    if (cur_card.getTreasure() == situation.player_id_ - 1 &&
        remaining <= settings_.reachableHomeCutoff) {
      score += (settings_.reachableHomeValue / remaining);
    }
  }

  return score;
}

int HeuristicStrategy::evaluate_position_score(const GameSituation& situation, const Position& position, int& situations) {
  int score = 0;

  auto& cur_card = situation.board_.cards_[position.row][position.col];

  bool treasureFound = false;

  if (position.row == -1 && position.col == -1) {
    score -= settings_.moveOutOfMapPenalty;
  }

  if (cur_card.getTreasure() == situation.treasure_) {
    score += settings_.onTreasureScore;
    treasureFound = true;
  } else {
    // distance score
    for (int row = 0; row < 7; ++row) {
      for (int col = 0; col < 7; ++col) {
        if (situation.board_.cards_[row][col].getTreasure() == situation.treasure_) {
          int distance = ::abs(position.row - row) + ::abs(position.col - col);

          score += settings_.distanceBaseValue / (distance * settings_.distanceFactor);
        };
      }
    }
  }

    // one additional move transition not accounting for other players
    auto moves = MoveCalculator::get_possible_moves(situation);

    for (auto move: moves) {
      GameSituation cur_sit = situation;
      cur_sit.perform_shift(move);
      auto positions = MoveCalculator::get_possible_positions(cur_sit);

      for (auto pos: positions) {
        if (treasureFound) {
          if (cur_sit.board_.cards_[pos.row][pos.col].getTreasure() > 3 &&
              cur_sit.found_treasures_.find(cur_sit.board_.cards_[pos.row][pos.col].getTreasure()) ==
              cur_sit.found_treasures_.end()) {
            score += settings_.transitionPossibleTreasureValue;
          }
        } else {
          if (cur_sit.board_.cards_[pos.row][pos.col].getTreasure() == situation.treasure_) {
            score += settings_.transitionMoveValue;

            auto next_moves = MoveCalculator::get_possible_moves(situation);
            GameSituation copy_sit = cur_sit;
            copy_sit.perform_shift(move);
            auto next_positions = MoveCalculator::get_possible_positions(copy_sit);
            for (auto next_pos : positions) {
              if (copy_sit.board_.cards_[next_pos.row][next_pos.col].getTreasure() == situation.treasure_) {
                score += settings_.transitionMoveValue / 2;
              }
            }
          }
        }
        situations++;
      }

  }

  return score;
}

int HeuristicStrategy::evaluate_enemy_score(const GameSituation& situation, int& situations) {
  int score = 0;

  // best player score
  int best_id = 0;
  int best_remaining = INT32_MAX;
  for (int i = 0; i < situation.player_count_; ++i) {
    if ((i + 1) != situation.player_id_ && situation.players_[i].remainingTreasures_ < best_remaining) {
      best_id = i + 1;
      best_remaining = situation.players_[i].remainingTreasures_;
    }
  }

  if (best_id > 0) {
    int best_score = 0;
    GameSituation new_sit = situation;
    new_sit.found_treasures_.insert(situation.treasure_);
    new_sit.player_id_ = best_id;
    auto positions = MoveCalculator::get_possible_positions(new_sit);
    for (auto pos : positions) {
      situations++;
      if (new_sit.board_.cards_[pos.row][pos.col].getTreasure() > 3 &&
          new_sit.found_treasures_.find(new_sit.board_.cards_[pos.row][pos.col].getTreasure()) ==
          new_sit.found_treasures_.end()) {
        best_score += settings_.bestReachableTreasureValue;
      }
    }


    auto moves = MoveCalculator::get_possible_moves(new_sit);
    for (auto move : moves) {
      GameSituation transitionSit = new_sit;

      transitionSit.perform_shift(move);

      auto trans_positions = MoveCalculator::get_possible_positions(transitionSit);

      for (auto pos : trans_positions) {
        situations++;
        if (new_sit.board_.cards_[pos.row][pos.col].getTreasure() > 3 &&
            new_sit.found_treasures_.find(new_sit.board_.cards_[pos.row][pos.col].getTreasure()) ==
            new_sit.found_treasures_.end()) {
          best_score += settings_.bestTransitionMoveTreasureValue;

          Position home = new_sit.players_[best_id - 1].home_;
          if ((best_remaining <= settings_.bestReachableHomeCutoff) &&
              (home.row == pos.row && home.col == pos.col)) {
            best_score += settings_.bestReachableHomeValue;
          }
        }
      }
    }

    score += best_score / best_remaining; // adjust for remaining treasure scaling
  }

  // next player score
  int next_id = (situation.player_id_ % situation.player_count_) + 1;

  if (next_id != situation.player_id_) {
    int next_score = 0;

    GameSituation new_sit = situation;
    new_sit.found_treasures_.insert(situation.treasure_);
    new_sit.player_id_ = next_id;
    auto positions = MoveCalculator::get_possible_positions(new_sit);
    for (auto pos : positions) {
      situations++;
      if (new_sit.board_.cards_[pos.row][pos.col].getTreasure() > 3 &&
          new_sit.found_treasures_.find(new_sit.board_.cards_[pos.row][pos.col].getTreasure()) ==
          new_sit.found_treasures_.end()) {
        next_score += settings_.nextReachableTreasureValue;
      }
    }

    auto moves = MoveCalculator::get_possible_moves(new_sit);
    for (auto move : moves) {
      GameSituation transitionSit = new_sit;

      transitionSit.perform_shift(move);

      auto trans_positions = MoveCalculator::get_possible_positions(transitionSit);

      for (auto pos : trans_positions) {
        situations++;
        if (new_sit.board_.cards_[pos.row][pos.col].getTreasure() > 3 &&
            new_sit.found_treasures_.find(new_sit.board_.cards_[pos.row][pos.col].getTreasure()) ==
            new_sit.found_treasures_.end()) {
          next_score += settings_.nextTransitionMoveTreasureValue;
        }

        Position home = new_sit.players_[best_id - 1].home_;
        if ((situation.players_[next_id - 1].remainingTreasures_ <= settings_.nextReachableHomeCutoff) &&
            (home.row == pos.row && home.col == pos.col)) {
          next_score += settings_.nextReachableHomeValue;
        }
      }
    }

    score += next_score / situation.players_[next_id - 1].remainingTreasures_; // adjust for remaining treasure scaling
  }

  return score;
}
