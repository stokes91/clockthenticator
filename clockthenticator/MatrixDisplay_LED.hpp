
#ifndef MATRIX_DISPLAY_H
#define MATRIX_DISPLAY_H

#include <Adafruit_Protomatter.h>

#define HEIGHT  32 // Matrix height (pixels) - SET TO 64 FOR 64x64 MATRIX!
#define WIDTH   64 // Matrix width (pixels)

uint8_t rgbPins[]  = {42, 41, 40, 38, 39, 37};
uint8_t addrPins[] = {45, 36, 48, 35, 21};
uint8_t clockPin   = 2;
uint8_t latchPin   = 47;
uint8_t oePin      = 14;

#if HEIGHT == 16
  #define NUM_ADDR_PINS 3
#elif HEIGHT == 32
  #define NUM_ADDR_PINS 4
#elif HEIGHT == 64
  #define NUM_ADDR_PINS 5
#endif

Adafruit_Protomatter _matrix(
  WIDTH, 4, 1, rgbPins, NUM_ADDR_PINS, addrPins,
  clockPin, latchPin, oePin, true);

class MatrixDisplay {
public:

  MatrixDisplay() { 

  }

  void setup() {
    ProtomatterStatus status = _matrix.begin();
  }

  void drawPixel(uint8_t x, uint8_t y, bool on) {
    // Hook this to your LED matrix API
    
    _matrix.drawPixel(x, y, on ? _matrix.color565(255, 255, 255) : _matrix.color565(0, 0, 0));
  }

  void fillScreen(bool on) {
    _matrix.fillScreen(on ? _matrix.color565(255, 255, 255) : _matrix.color565(0, 0, 0));
  }

  void show() {
    _matrix.show();
  }

};

#endif // MATRIX_DISPLAY_H