#ifndef MATRIX_DISPLAY_H
#define MATRIX_DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



class MatrixDisplay {
public:

  MatrixDisplay() { 

  }

  void setup() {
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!_display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
  }

  void drawPixel(uint8_t x, uint8_t y, bool on) {
    _display.drawPixel(x * 2, y * 2, on ? SSD1306_WHITE : SSD1306_BLACK);
    _display.drawPixel(x * 2 + 1, y * 2, on ? SSD1306_WHITE : SSD1306_BLACK);
    _display.drawPixel(x * 2, y * 2 + 1, on ? SSD1306_WHITE : SSD1306_BLACK);
    _display.drawPixel(x * 2 + 1, y * 2 + 1, on ? SSD1306_WHITE : SSD1306_BLACK);
  }

  void fillScreen(bool on) {
    _display.clearDisplay();
    _display.fillRect(0, 0, _display.width()-1, _display.height()-1, on ? SSD1306_WHITE : SSD1306_BLACK);
  }

  void show() {
    _display.display(); 
  }

};

#endif // MATRIX_DISPLAY_H