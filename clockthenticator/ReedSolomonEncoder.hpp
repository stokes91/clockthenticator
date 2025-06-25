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