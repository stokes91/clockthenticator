#ifndef BARCODE_CANVAS_H
#define BARCODE_CANVAS_H

#include "QRConstants.h"
#include "Coord.hpp"

class BarcodeCanvas {
public:
  Coord coords[MAX_COORDS];
  int coordsCount;

  BarcodeCanvas() : coordsCount(0) { }

  void reset() {
    coordsCount = 0;
  }

  void setDataModule(int col, int row) {
    if (coordsCount < MAX_COORDS)
      coords[coordsCount++] = Coord(col, row);
  }


  void toMatrix(void (*drawPixel)(uint8_t, uint8_t, bool)) {
    for (int i = 0; i < coordsCount; i++) {
      drawPixel(coords[i].x, coords[i].y, true);
    }
  }
};

#endif // BARCODE_CANVAS_H