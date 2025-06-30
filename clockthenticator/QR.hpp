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

#include <math.h>
#include <stdio.h>
#include <string.h>

#define MAX_COORDS       1024   // maximum number of coordinates
#define MAX_MATRIX_SIDE  64     // maximum matrix side length (for drawMatrix)
#define EDGE_LENGTH      25     // fixed edge length for ModuleCanvas
#define MAX_RUNS         64     // maximum run segments in ColorRun
#define MAX_BLOCKS_BITS  1024   // maximum bits in CodewordEncoder blocks
#define MAX_CODEWORDS    128    // maximum codewords produced
#define MAX_ECC          16     // maximum ECC bytes
#define MAX_INPUT_SIZE   64     // maximum input size in bytes

#include "Coord.hpp"
#include "BarcodeCanvas.hpp"
#include "ColorRun.hpp"
#include "QRUtils.hpp"
#include "ReedSolomonEncoder.hpp"
#include "CodewordEncoder.hpp"
#include "ModuleCanvas.hpp"

class QR {
public:
  ModuleCanvas moduleCanvas;
  CodewordEncoder codewordEncoder;
  int allCodewords[MAX_CODEWORDS];
  int allCodewordsCount;

  QR() : moduleCanvas(-1), allCodewordsCount(0) { }

  QR& appendBinarySegment(const uint8_t text[], int textLength) {
    
    codewordEncoder.appendBinarySegment(text, textLength);
    return *this;
  }
  
  void setNotMaskId(int notMaskId) {
    moduleCanvas.setNotMaskId(notMaskId);
  }

  int getBestMaskId() const { return moduleCanvas.getBestMaskId(); }

  void toMatrix(void (*drawPixel)(uint8_t, uint8_t, bool)) {
    int cw[MAX_CODEWORDS];
    int cwCount = 0;
    codewordEncoder.toArray(cw, &cwCount);
    moduleCanvas.placeDataBits(cw, cwCount);
    moduleCanvas.toMatrix(drawPixel);
  }
  
};