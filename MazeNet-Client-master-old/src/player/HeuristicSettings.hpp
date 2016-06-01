#ifndef MAZENET_CLIENT_HEURISTICSETTINGS_HPP
#define MAZENET_CLIENT_HEURISTICSETTINGS_HPP

#include <util/cfg/CfgManager.hpp>

struct HeuristicSettings {
  HeuristicSettings() {
    using namespace mazenet::util::cfg;
    CfgManager& cfgMan = CfgManager::instance();
    reachableFieldValue = cfgMan.get<int>("heuristic.base.reachableFieldValue");
    reachableTreasureValue = cfgMan.get<int>("heuristic.base.reachableTreasureValue");
    reachableHomeValue = cfgMan.get<int>("heuristic.base.reachableHomeValue");
    reachableHomeCutoff = cfgMan.get<int>("heuristic.base.reachableHomeCutoff");

    onTreasureScore = cfgMan.get<int>("heuristic.position.onTargetValue");
    transitionMoveValue = cfgMan.get<int>("heuristic.position.transitionMoveValue");
    transitionPossibleTreasureValue = cfgMan.get<int>("heuristic.position.transitionPossibleTreasureValue");
    distanceBaseValue = cfgMan.get<int>("heuristic.position.distanceBaseValue");
    distanceFactor = cfgMan.get<int>("heuristic.position.distanceFactor");
    moveOutOfMapPenalty = cfgMan.get<int>("heuristic.position.moveOutOfMapPenalty");

    nextReachableTreasureValue = cfgMan.get<int>("heuristic.enemy.next.reachableTreasureValue");
    nextReachableHomeValue = cfgMan.get<int>("heuristic.enemy.next.reachableHomeValue");
    nextReachableHomeCutoff = cfgMan.get<int>("heuristic.enemy.next.reachableHomeCutoff");
    nextTransitionMoveTreasureValue = cfgMan.get<int>("heuristic.enemy.next.transitionMoveTreasureValue");

    bestReachableTreasureValue = cfgMan.get<int>("heuristic.enemy.best.reachableTreasureValue");
    bestReachableHomeValue = cfgMan.get<int>("heuristic.enemy.best.reachableHomeValue");
    bestReachableHomeCutoff = cfgMan.get<int>("heuristic.enemy.best.reachableHomeCutoff");
    bestTransitionMoveTreasureValue = cfgMan.get<int>("heuristic.enemy.best.transitionMoveTreasureValue");

  }

  int reachableFieldValue = 0;
  int reachableTreasureValue = 0;
  int reachableHomeValue = 0;
  int reachableHomeCutoff = 0;

  int onTreasureScore = 0;
  int transitionMoveValue = 0;
  int transitionPossibleTreasureValue = 0;
  int distanceBaseValue = 0;
  int distanceFactor = 0;
  int moveOutOfMapPenalty = 0;

  int nextReachableTreasureValue = 0;
  int nextReachableHomeValue = 0;
  int nextReachableHomeCutoff = 0;
  int nextTransitionMoveTreasureValue = 0;

  int bestReachableTreasureValue = 0;
  int bestReachableHomeValue = 0;
  int bestReachableHomeCutoff = 0;
  int bestTransitionMoveTreasureValue = 0;
};


#endif //MAZENET_CLIENT_HEURISTICSETTINGS_HPP
