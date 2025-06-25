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



/*

Map the digit to a seven segment output


*/

#ifndef SEVEN_SEGMENT_MATRIX_HPP
#define SEVEN_SEGMENT_MATRIX_HPP

#include <Arduino.h>

#define DIGIT_SEP_DATE 11
#define DIGIT_SEP_TIME 10

const uint8_t digitBitmaps[12][7] = {
  // Each array represents a 3x7 grid, one byte per row (LSB = leftmost pixel)
  { 0b111, 0b101,0b101, 0b101, 0b101,0b101, 0b111 }, // 0
  { 0b001, 0b001,0b001, 0b001, 0b001,0b001, 0b001 }, // 1
  { 0b111, 0b001,0b001, 0b111, 0b100,0b100, 0b111 }, // 2
  { 0b111, 0b001,0b001, 0b111, 0b001,0b001, 0b111 }, // 3
  { 0b101, 0b101,0b101, 0b111, 0b001,0b001, 0b001 }, // 4
  { 0b111, 0b100,0b100, 0b111, 0b001,0b001, 0b111 }, // 5
  { 0b111, 0b100,0b100, 0b111, 0b101,0b101, 0b111 }, // 6
  { 0b111, 0b001,0b001, 0b001, 0b001,0b001, 0b001 }, // 7
  { 0b111, 0b101,0b101, 0b111, 0b101,0b101, 0b111 }, // 8
  { 0b111, 0b101,0b101, 0b111, 0b001,0b001, 0b111 }, // 9
  { 0b000, 0b010,0b000, 0b000, 0b000,0b010, 0b000 }, // :
  { 0b000, 0b000,0b000, 0b010, 0b000,0b000, 0b000 }, // .
};

// Draw a digit at (x0, y0) with your own pixel-drawing function
void drawDigit(uint8_t digit, uint8_t x0, uint8_t y0, void (*drawPixel)(uint8_t, uint8_t, bool)) {
  if (digit > 12) return;
  for (uint8_t y = 0; y < 7; y++) {
    uint8_t row = digitBitmaps[digit][y];
    for (uint8_t x = 0; x < 3; x++) {
      bool on = bitRead(row, 2 - x); // 2-x to match left-to-right
      drawPixel(x0 + x, y0 + y, on);
    }
  }
}

void drawDigitDoubled(uint8_t digit, uint8_t x0, uint8_t y0, void (*drawPixel)(uint8_t, uint8_t, bool)) {
  if (digit > 12) return;
  for (uint8_t y = 0; y < 7; y++) {
    uint8_t row = digitBitmaps[digit][y];
    for (uint8_t x = 0; x < 3; x++) {
      bool on = bitRead(row, 2 - x); // 2-x to match left-to-right
      drawPixel(x0 + x*2, y0 + y*2, on);
      drawPixel(x0 + x*2 + 1, y0 + y*2, on);  
      drawPixel(x0 + x*2, y0 + y*2  + 1, on);
      drawPixel(x0 + x*2 + 1, y0 + y*2 + 1, on);  
    }
  }
}

#endif // SEVEN_SEGMENT_MATRIX_HPP