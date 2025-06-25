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

#ifndef GALOIS_FIELD_H
#define GALOIS_FIELD_H

#include "QRConstants.h"

class GaloisField {
  public:
    int length;
    int generator;
    int base;
    int expTable[256];
    int logTable[256];
    
    GaloisField(int size, int generator, int base)
        : length(0), generator(generator), base(base) {
      int j = 1;
      for (int i = 0; i < size; i++) {
        expTable[i] = j;
        j = j << 1;
        if (j >= size)
          j ^= generator;
      }
      for (int i = 0; i < size - 1; i++)
        logTable[expTable[i]] = i;
      length = size - 1;
    }
    
    int multiply(int x, int y) const {
      if (x == 0 || y == 0)
        return 0;
      return expTable[(logTable[x] + logTable[y]) % length];
    }
    
    int invert(int x) const {
      return expTable[length - logTable[x]];
    }
    
    int divide(int x, int y) const {
      return multiply(x, invert(y));
    }
    
    int logVal(int x) const { return logTable[x]; }
    int expVal(int x) const { return expTable[x]; }
    
    static bool IsZero(int x) { return x == 0; }
    static bool IsOne(int x) { return x == 1; }
    static int Add(int x, int y) { return x ^ y; }
  };

#endif // GALOIS_FIELD_H