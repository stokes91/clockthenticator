// DotMatrixDisplay.hpp
#ifndef DOT_MATRIX_DISPLAY_HPP
#define DOT_MATRIX_DISPLAY_HPP

#include <Arduino.h>

// Seven-segment symbol dimensions
const uint8_t SYMBOL_WIDTH  = 3;
const uint8_t SYMBOL_HEIGHT = 7;
const uint8_t SYMBOL_COUNT  = 10 + 26 + 1 + 1 + 1;

const uint8_t symbolBitmaps[SYMBOL_COUNT][SYMBOL_HEIGHT] = {
  // 0–9
  {0b111,0b101,0b101,0b101,0b101,0b101,0b111},
  {0b001,0b001,0b001,0b001,0b001,0b001,0b001},
  {0b111,0b001,0b001,0b111,0b100,0b100,0b111},
  {0b111,0b001,0b001,0b111,0b001,0b001,0b111},
  {0b101,0b101,0b101,0b111,0b001,0b001,0b001},
  {0b111,0b100,0b100,0b111,0b001,0b001,0b111},
  {0b111,0b100,0b100,0b111,0b101,0b101,0b111},
  {0b111,0b001,0b001,0b001,0b001,0b001,0b001},
  {0b111,0b101,0b101,0b111,0b101,0b101,0b111},
  {0b111,0b101,0b101,0b111,0b001,0b001,0b111},
  // A–Z
  {0b010,0b101,0b101,0b111,0b101,0b101,0b101}, // A
  {0b110,0b101,0b101,0b110,0b101,0b101,0b110}, // B
  {0b111,0b100,0b100,0b100,0b100,0b100,0b111}, // C
  {0b110,0b101,0b101,0b101,0b101,0b101,0b110}, // D
  {0b111,0b100,0b100,0b110,0b100,0b100,0b111}, // E
  {0b111,0b100,0b100,0b110,0b100,0b100,0b100}, // F
  {0b111,0b100,0b100,0b101,0b101,0b101,0b111}, // G
  {0b101,0b101,0b101,0b111,0b101,0b101,0b101}, // H
  {0b111,0b010,0b010,0b010,0b010,0b010,0b111}, // I
  {0b111,0b001,0b001,0b001,0b101,0b101,0b011}, // J
  {0b100,0b100,0b101,0b110,0b100,0b110,0b101}, // K
  {0b100,0b100,0b100,0b100,0b100,0b100,0b111}, // L
  {0b101,0b111,0b111,0b101,0b101,0b101,0b101}, // M
  {0b101,0b111,0b111,0b111,0b111,0b101,0b101}, // N
  {0b111,0b101,0b101,0b101,0b101,0b101,0b111}, // O
  {0b111,0b101,0b101,0b111,0b100,0b100,0b100}, // P
  {0b111,0b101,0b101,0b101,0b101,0b011,0b001}, // Q
  {0b111,0b101,0b101,0b111,0b110,0b101,0b101}, // R
  {0b111,0b100,0b100,0b111,0b001,0b001,0b111}, // S
  {0b111,0b010,0b010,0b010,0b010,0b010,0b010}, // T
  {0b101,0b101,0b101,0b101,0b101,0b101,0b111}, // U
  {0b101,0b101,0b101,0b101,0b101,0b101,0b010}, // V
  {0b101,0b101,0b101,0b101,0b111,0b111,0b101}, // W
  {0b101,0b101,0b010,0b010,0b010,0b101,0b101}, // X
  {0b101,0b101,0b101,0b010,0b010,0b010,0b010}, // Y
  {0b111,0b001,0b010,0b100,0b100,0b100,0b111}, // Z
  // m
  {0b000,0b000,0b101,0b111,0b101,0b101,0b101},
  // ':' and '.' Half character width.
  {0b000,0b100,0b000,0b000,0b000,0b100,0b000},
  {0b000,0b000,0b000,0b100,0b000,0b000,0b000}
};

inline uint8_t charToIndex(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'Z') return 10 + (c - 'A');
  if (c == 'm')              return 10 + 26;
  if (c == ':')              return 10 + 26 + 1;
  if (c == '-')              return 10 + 26 + 2;
  return -1;
}


// Draw a digit at (x0, y0) with your own pixel-drawing function
void drawDigit(uint8_t digit, uint8_t x0, uint8_t y0, void (*drawPixel)(uint8_t, uint8_t, bool)) {
  if (digit > SYMBOL_COUNT) return;
  for (uint8_t y = 0; y < 7; y++) {
    uint8_t row = symbolBitmaps[digit][y];
    for (uint8_t x = 0; x < 3; x++) {
      bool on = bitRead(row, 2 - x); // 2-x to match left-to-right
      drawPixel(x0 + x, y0 + y, on);
    }
  }
}

void drawDigitDoubled(uint8_t digit, uint8_t x0, uint8_t y0, void (*drawPixel)(uint8_t, uint8_t, bool)) {
  if (digit > SYMBOL_COUNT) return;
  for (uint8_t y = 0; y < 7; y++) {
    uint8_t row = symbolBitmaps[digit][y];
    for (uint8_t x = 0; x < 3; x++) {
      bool on = bitRead(row, 2 - x); // 2-x to match left-to-right
      drawPixel(x0 + x*2, y0 + y*2, on);
      drawPixel(x0 + x*2 + 1, y0 + y*2, on);  
      drawPixel(x0 + x*2, y0 + y*2  + 1, on);
      drawPixel(x0 + x*2 + 1, y0 + y*2 + 1, on);  
    }
  }
}

int penX;
int penY;

void move(int x, int y) {
  penX = x;
  penY = y;
}

void writeDigits(uint32_t date_coded_year, uint8_t digits, void (*drawPixel)(uint8_t, uint8_t, bool)) {


  for (uint8_t l = digits, i = 0; l--; i++) {
    uint8_t digit = date_coded_year % 10;
    date_coded_year -= digit;
    date_coded_year /= 10;
  
    drawDigit(digit, l * 4 + penX, penY, drawPixel);
  }
  
  penX += digits * 4;

}

void writeDigitsDoubled(uint32_t date_coded_year, uint8_t digits, void (*drawPixel)(uint8_t, uint8_t, bool)) {


  for (uint8_t l = digits, i = 0; l--; i++) {
    uint8_t digit = date_coded_year % 10;
    date_coded_year -= digit;
    date_coded_year /= 10;
  
    drawDigitDoubled(digit, l * 8 + penX, penY, drawPixel);
  }

  penX += digits * 8;

}

void writeText(const char* text, int length, void (*drawPixel)(uint8_t, uint8_t, bool)) {

  for (int l = length, i = 0; l--; i++) {
    uint8_t digit = charToIndex(text[i]);
    drawDigit(digit, penX, penY, drawPixel);
    penX += digit > 10 + 26 ? 2 : 4;
  }

}

void writeTextDoubled(const char* text, int length, void (*drawPixel)(uint8_t, uint8_t, bool)) {

  for (int l = length, i = 0; l--; i++) {
    uint8_t digit = charToIndex(text[i]);
    drawDigitDoubled(digit, penX, penY, drawPixel);
    penX += digit > 10 + 26 ? 5 : 8;
  }

}

#endif // DOT_MATRIX_DISPLAY_HPP
