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

#ifndef REED_SOLOMON_ENCODER_H
#define REED_SOLOMON_ENCODER_H

#include "GaloisField.hpp"
#include "QRConstants.h"

class ReedSolomonEncoder {
public:
  GaloisField field;
  int coefficients[MAX_ECC];
  int eccCount;
  
  ReedSolomonEncoder(const GaloisField &field, int BLOCKS_ECC)
      : field(field), eccCount(BLOCKS_ECC) {
    for (int i = 0; i < BLOCKS_ECC; i++)
      coefficients[i] = 0;
    coefficients[BLOCKS_ECC - 1] = 1;
    int root = field.base;
    for (int i = 0; i < BLOCKS_ECC; i++) {
      for (int j = 0; j < BLOCKS_ECC; j++) {
        coefficients[j] = field.multiply(coefficients[j], root);
        if (j + 1 < BLOCKS_ECC)
          coefficients[j] = GaloisField::Add(coefficients[j], coefficients[j+1]);
      }
      root = field.multiply(root, 2);
    }
  }
  
  void encode(const int data[], int dataCount, int output[], int *outputCount) {
    for (int i = 0; i < dataCount; i++)
      output[i] = data[i];
    int k = eccCount;
    int ecc[MAX_ECC] = {0};
    for (int i = 0; i < dataCount; i++) {
      int factor = GaloisField::Add(data[i], ecc[0]);
      for (int j = 0; j < k - 1; j++)
        ecc[j] = ecc[j+1];
      ecc[k-1] = 0;
      for (int j = 0; j < k; j++)
        ecc[j] = GaloisField::Add(ecc[j], field.multiply(coefficients[j], factor));
    }
    for (int i = 0; i < k; i++)
      output[dataCount + i] = ecc[i];
    *outputCount = dataCount + k;
  }
};


#endif // REED_SOLOMON_ENCODER_H