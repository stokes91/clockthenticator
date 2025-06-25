#ifndef COORD_H
#define COORD_H

class Coord {
public:
  uint8_t x;
  uint8_t y;

  Coord() : x(0), y(0) {}
  Coord(uint8_t x, uint8_t y) : x(x), y(y) {}
};

#endif // COORD_H