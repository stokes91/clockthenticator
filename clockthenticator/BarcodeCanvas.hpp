// Copyright 2025 Alexander Stokes
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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