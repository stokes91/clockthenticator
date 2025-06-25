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

#ifndef MODULE_CANVAS_H
#define MODULE_CANVAS_H

#include "QRConstants.h"
#include "BarcodeCanvas.hpp"
#include "ColorRun.hpp"
#include "QRUtils.hpp"
#include <stdint.h>
#include <math.h>

class ModuleCanvas {
public:
  uint8_t rawCanvas[EDGE_LENGTH * EDGE_LENGTH];
  uint8_t bestMaskedCanvas[EDGE_LENGTH * EDGE_LENGTH];
  bool bestMaskedCanvasSet;
  int lowestPenalty;
  int bestMaskId;
  
  int edgeLength;
  int version;
  int errorCorrectionLevel;
  
  BarcodeCanvas barcodeCanvas;
  
  ModuleCanvas(int forcedMaskId = -1)
    : bestMaskedCanvasSet(false), lowestPenalty(0), bestMaskId(forcedMaskId),
    edgeLength(EDGE_LENGTH), version(2), errorCorrectionLevel(1) {
    for (int i = 0; i < edgeLength * edgeLength; i++)
      rawCanvas[i] = 0;
    barcodeCanvas.reset();
    lowestPenalty = 0x7fffffff;
    placeFunctionPatterns();
  }
  
  void reset() {
    bestMaskedCanvasSet = false;
    lowestPenalty = 0;
    bestMaskId = -1;
    edgeLength = EDGE_LENGTH;
    version = 2;
    errorCorrectionLevel = 1;
  
    for (int i = 0; i < edgeLength * edgeLength; i++)
      rawCanvas[i] = 0;
    barcodeCanvas.reset();
    lowestPenalty = 0x7fffffff;
    placeFunctionPatterns();
  }
  
  void placeDataBits(const int allCodewords[], int codewordsCount) {
    reset();
    drawFormatBits(0);
    int bitIndex = 0;
    int totalBits = codewordsCount * 8;
    for (int rightCol = edgeLength - 2; rightCol >= 1; rightCol -= 2) {
      for (int rowStep = 0; rowStep < edgeLength - 1; rowStep++) {
        for (int subCol = 0; subCol < 2; subCol++) {
          int x = rightCol - subCol;
          bool upward = (((rightCol + 1) & 2) == 0);
          int y = upward ? (edgeLength - 2 - rowStep) : rowStep;
          if (y >= 6) y++;
          if (x >= 6) x++;
          if (!isFunctionModule(x, y) && bitIndex < totalBits) {
            int theByte = allCodewords[bitIndex >> 3];
            int bit = (theByte >> (7 - (bitIndex & 7))) & 1;
            int idx = y * edgeLength + x;
            if (bit == 1)
              rawCanvas[idx] = 1;
            bitIndex++;
          }
        }
      }
    }
    if (bestMaskId >= 0) {
      drawFormatBits(bestMaskId);
      applyMaskAndEvaluate(bestMaskId);
    } else {
      for (int candidateMask = 0; candidateMask < 8; candidateMask++) {
        drawFormatBits(candidateMask);
        applyMaskAndEvaluate(candidateMask);
      }
    }
  
    if (!bestMaskedCanvasSet)
      return;
    int quiet = 4;
    for (int row = 0; row < edgeLength; row++) {
      for (int col = 0; col < edgeLength; col++) {
        int idx = row * edgeLength + col;
        int color = (bestMaskedCanvas[idx] & 0x1) ? 1 : 0;
        if (!color) continue;
        barcodeCanvas.setDataModule(col + quiet, row + quiet);
      }
    }
  }
  
  int getMaskId() const { return bestMaskId; }
  
  void toMatrix(void (*drawPixel)(uint8_t, uint8_t, bool)) {
    barcodeCanvas.toMatrix(drawPixel);
  }

  
private:
  void applyMaskAndEvaluate(int maskId) {
    uint8_t maskedCanvas[EDGE_LENGTH * EDGE_LENGTH];
    for (int y = 0; y < edgeLength; y++) {
      for (int x = 0; x < edgeLength; x++) {
        int idx = y * edgeLength + x;
        if (isFunctionModule(x, y))
          maskedCanvas[idx] = rawCanvas[idx] & 1;
        else {
          int maskBit = GetMaskBit(maskId, y, x) ? 1 : 0;
          maskedCanvas[idx] = (rawCanvas[idx] ^ maskBit) & 1;
        }
      }
    }
    int penalty = computePenalty(maskedCanvas);
    if (penalty < lowestPenalty) {
      lowestPenalty = penalty;
      bestMaskId = maskId;
      for (int i = 0; i < edgeLength * edgeLength; i++)
        bestMaskedCanvas[i] = maskedCanvas[i];
      bestMaskedCanvasSet = true;
    }
  }
  
  int computePenalty(const uint8_t canvas[]) {
    int penaltyRuns = 0, penaltyFinders = 0;
    for (int r = 0; r < edgeLength; r++) {
      ColorRun rowRun, colRun;
      for (int c = 0; c < edgeLength; c++) {
        rowRun.append(canvas[r * edgeLength + c]);
        colRun.append(canvas[c * edgeLength + r]);
      }
      int rowCount = rowRun.finish();
      int colCount = colRun.finish();
      penaltyFinders += computeFindPenalty(rowRun.runs, rowCount) +
                computeFindPenalty(colRun.runs, colCount);
      penaltyRuns += computeRunPenalty(rowRun.runs, rowCount) +
               computeRunPenalty(colRun.runs, colCount);
    }
    int penaltyTwoByTwo = 0;
    for (int y = 0; y < edgeLength - 1; y++) {
      for (int x = 0; x < edgeLength - 1; x++) {
        int c = canvas[y * edgeLength + x];
        if (c == canvas[y * edgeLength + (x + 1)] &&
          c == canvas[(y + 1) * edgeLength + x] &&
          c == canvas[(y + 1) * edgeLength + (x + 1)])
          penaltyTwoByTwo += 3;
      }
    }
    int totalModules = edgeLength * edgeLength;
    int darkCount = 0;
    for (int i = 0; i < totalModules; i++) {
      if (canvas[i] == 1)
        darkCount++;
    }
    float darkPercent = (darkCount * 100.0f) / totalModules;
    int penaltyDarkBalance = 10 * static_cast<int>(floor(fabs(darkPercent - 50) / 5));
    return penaltyRuns + penaltyFinders + penaltyTwoByTwo + penaltyDarkBalance;
  }
  
  void drawFormatBits(int mask) {
    int formatData = (errorCorrectionLevel << 3) | mask;
    if (version >= 7) {
      int versionRem = version;
      for (int i = 0; i < 12; i++)
        versionRem = (versionRem << 1) ^ ((versionRem >> 11) * 0x1F25);
      int versionBits = (version << 12) | versionRem;
      for (int i = 0; i < 18; i++) {
        int moduleColor = (versionBits >> i) & 1;
        int a = edgeLength - 11 + (i % 3);
        int b = i / 3;
        setVersionBit(a, b, moduleColor);
        setVersionBit(b, a, moduleColor);
      }
    }
    int eccRem = formatData;
    for (int i = 0; i < 10; i++)
      eccRem = (eccRem << 1) ^ ((eccRem >> 9) * 0x537);
    int formatBits = ((formatData << 10) | eccRem) ^ 0x5412;
    for (int i = 0; i <= 5; i++)
      setMaskedFunctionModule(8, i, (formatBits >> i) & 1);
    setMaskedFunctionModule(8, 7, (formatBits >> 6) & 1);
    setMaskedFunctionModule(8, 8, (formatBits >> 7) & 1);
    setMaskedFunctionModule(7, 8, (formatBits >> 8) & 1);
    for (int i = 9; i < 15; i++)
      setMaskedFunctionModule(14 - i, 8, (formatBits >> i) & 1);
    for (int i = 0; i < 8; i++)
      setMaskedFunctionModule(edgeLength - 1 - i, 8, (formatBits >> i) & 1);
    for (int i = 8; i < 15; i++)
      setMaskedFunctionModule(8, edgeLength - 15 + i, (formatBits >> i) & 1);
    setMaskedFunctionModule(8, edgeLength - 8, 1);
  }
  
  void setMaskedFunctionModule(int x, int y, int color) {
    int idx = y * edgeLength + x;
    rawCanvas[idx] = (color & 1) | (1 << 2);
  }
  
  void setVersionBit(int x, int y, int color) {
    int idx = y * edgeLength + x;
    rawCanvas[idx] = (color & 1) | (1 << 2);
  }
  
  bool isFunctionModule(int x, int y) {
    int idx = y * edgeLength + x;
    return ((rawCanvas[idx] >> 2) & 1) == 1;
  }
  
  void placeFunctionPatterns() {
    placeFinderPattern(3, 3);
    placeFinderPattern(edgeLength - 4, 3);
    placeFinderPattern(3, edgeLength - 4);
    for (int i = 8; i < edgeLength - 8; i++) {
      int isBlack = (i % 2 == 0) ? 1 : 0;
      setVersionBit(6, i, isBlack);
      setVersionBit(i, 6, isBlack);
    }
    if (version > 1) {
      int alignmentCount = (version / 7) + 2;
      int step = static_cast<int>(floor((version * 8 + alignmentCount * 3 + 5) /
            float(alignmentCount * 4 - 4)) * 2);
      int alignPos[16];
      int alignCount = 0;
      alignPos[alignCount++] = 6;
      for (int pos = edgeLength - 7; alignCount < alignmentCount; pos -= step) {
        for (int j = alignCount; j > 1; j--)
          alignPos[j] = alignPos[j-1];
        alignPos[1] = pos;
        alignCount++;
      }
      for (int i = 0; i < alignCount; i++) {
        for (int j = 0; j < alignCount; j++) {
          bool corner1 = (i == 0 && j == 0);
          bool corner2 = (i == 0 && j == alignCount - 1);
          bool corner3 = (i == alignCount - 1 && j == 0);
          if (!(corner1 || corner2 || corner3))
            placeAlignmentPattern(alignPos[i], alignPos[j]);
        }
      }
    }
  }
  
  void placeAlignmentPattern(int centerX, int centerY) {
    for (int dy = -2; dy <= 2; dy++) {
      for (int dx = -2; dx <= 2; dx++) {
        int dist = (dx < 0 ? -dx : dx);
        int t = (dy < 0 ? -dy : dy);
        if (t > dist) dist = t;
        int isBlack = (dist != 1) ? 1 : 0;
        setVersionBit(centerX + dx, centerY + dy, isBlack);
      }
    }
  }
  
  void placeFinderPattern(int centerX, int centerY) {
    for (int dy = -4; dy <= 4; dy++) {
      for (int dx = -4; dx <= 4; dx++) {
        int dist = (dx < 0 ? -dx : dx);
        int t = (dy < 0 ? -dy : dy);
        if (t > dist) dist = t;
        int xx = centerX + dx, yy = centerY + dy;
        if (xx >= 0 && xx < edgeLength && yy >= 0 && yy < edgeLength)
          setVersionBit(xx, yy, (dist != 2 && dist != 4) ? 1 : 0);
      }
    }
  }
};

#endif // MODULE_CANVAS_H