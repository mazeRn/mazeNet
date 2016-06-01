#ifndef MAZENET_CLIENT_POSITION_HPP
#define MAZENET_CLIENT_POSITION_HPP

struct Position {
  Position();

  Position(int row, int col);

  int row = 0;
  int col = 0;
};

#endif //MAZENET_CLIENT_POSITION_HPP
