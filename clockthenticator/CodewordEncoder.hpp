#ifndef CODEWORD_ENCODER_H
#define CODEWORD_ENCODER_H

#include "QRConstants.h"
#include "ReedSolomonEncoder.hpp"  // Needed for ENCODER_CACHE in toArray()
#include <stdint.h>
#include <string.h>

// Global QR_FIELD instance
static GaloisField QR_FIELD(0x100, 0x11d, 1);

// Global encoder cache (for simplicity, only one element is used)
static ReedSolomonEncoder ENCODER_CACHE = ReedSolomonEncoder(QR_FIELD, 10);

class CodewordEncoder {
public:
  int blocks[MAX_BLOCKS_BITS];
  int blockCount;
  
  CodewordEncoder() : blockCount(0) { }
  
  void reset() {
    blockCount = 0;
  }
  
  CodewordEncoder& appendBinarySegment(const uint8_t text[], int textLength) {
    appendBits(0b0100, 4);
    appendBits(textLength, 8);

    for (int i = 0; i < textLength; i++)
      appendBits(text[i], 8);

    return *this;
  }
  
  int getBlockLength() const { return blockCount; }
  
  void toArray(int outCodewords[], int *outCount) {
    int codewordCount = 34;
    int blocksCount = 1;
    int totalDataBits = codewordCount * 8;
    int remaining = totalDataBits - blockCount;
    int terminatorBits = (remaining < 4) ? remaining : 4;
    for (int i = 0; i < terminatorBits && blockCount < MAX_BLOCKS_BITS; i++)
      blocks[blockCount++] = 0;
    while (blockCount % 8 != 0 && blockCount < MAX_BLOCKS_BITS)
      blocks[blockCount++] = 0;
    int codewords[MAX_CODEWORDS];
    int cwCount = 0;
    for (int i = 0; i < blockCount; i += 8) {
      int b = 0;
      for (int j = 0; j < 8; j++)
        b = (b << 1) | (blocks[i+j] & 1);
      codewords[cwCount++] = b;
    }
    int padBytes = codewordCount - (blockCount / 8);
    int pad = 1;
    for (int i = 0; i < padBytes; i++) {
      int val = pad ? 0xEC : 0x11;
      codewords[cwCount++] = val;
      pad = 1 ^ pad;
    }
    if (cwCount > codewordCount) {
      *outCount = 0;
      reset();
      return;
    }
    if (blocksCount <= 1) {
      int encoded[MAX_CODEWORDS];
      int encodedCount = 0;
      ENCODER_CACHE.encode(codewords, cwCount, encoded, &encodedCount);
      for (int i = 0; i < encodedCount; i++)
        outCodewords[i] = encoded[i];
      *outCount = encodedCount;
    } else {
      *outCount = 0; // Interleaving for multiple blocks not implemented.
    }
    reset();
  }
    
private:
  void appendBits(int val, int length) {
    for (int i = length - 1; i >= 0; i--) {
      if (blockCount < MAX_BLOCKS_BITS)
        blocks[blockCount++] = (val >> i) & 1;
    }
  }
};

#endif // CODEWORD_ENCODER_H