#ifndef COLOR_RUN_H
#define COLOR_RUN_H

#include "QRConstants.h"

class ColorRun {
public:
  int runs[MAX_RUNS];
  int runCount;
  int currentColor;
  int currentRunLength;
  
  ColorRun() : runCount(0), currentColor(128), currentRunLength(0) { }
  
  void append(int color) {
    if (currentColor == 128) {
      currentColor = color;
      currentRunLength = 1;
      return;
    }
    if (color == currentColor) {
      currentRunLength++;
    } else {
      if (runCount < MAX_RUNS)
        runs[runCount++] = currentRunLength;
      currentColor = color;
      currentRunLength = 1;
    }
  }
  
  int finish() {
    if (currentRunLength > 0 && runCount < MAX_RUNS)
      runs[runCount++] = currentRunLength;
    return runCount;
  }
};

inline int computeRunPenalty(const int runs[], int count) {
  int penalty = 0;
  for (int i = 0; i < count; i++) {
    if (runs[i] >= 5)
      penalty += 3 + (runs[i] - 5);
  }
  return penalty;
}

inline int computeFindPenalty(const int runs[], int count) {
  int penalty = 0;
  for (int i = 0; i < count - 6; i++) {
    int r0 = runs[i], r1 = runs[i+1], r2 = runs[i+2],
      r3 = runs[i+3], r4 = runs[i+4], r5 = runs[i+5],
      r6 = runs[i+6];
    if ((r1 == 1 && r2 == 3 && r3 == 1 && r4 == 1 && (r0 >= 4 || r5 >= 4)) ||
        (r1 == 1 && r2 == 1 && r3 == 3 && r4 == 1 && r5 == 1 && (r0 >= 4 || r6 >= 4)))
      penalty += 40;
  }
  return penalty;
}

#endif // COLOR_RUN_H