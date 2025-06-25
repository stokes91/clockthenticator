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


#include "Configuration.hpp"
// Configuration.hpp contains the following:
// timezoneOffsetHours
// hostnameSectionLength
// secretBase32 which is needed for HOTP

#include "HOTP.hpp"
#include "Coord.hpp"
#include "QR.hpp"
#include "SevenSeg.hpp"

/*  */

/* LED Matrix */
#include "MatrixDisplay_LED.hpp"

/* */

/* LCD Matrix 

#include "MatrixDisplay_LCD.hpp"

*/

#include "RTWPreferences.hpp"
#include "SwitchControl.hpp"
#include "DateUtil.hpp"

#include <Wire.h>

#include <RTClib.h>

/*

Board manager URL:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json

Install:
esp32 by Espressif Systems

*/


#define MAX_FPS 10 // Maximum redraw rate, frames/second

#define BUTTON_DEBOUNCE_MS 100
#define CLOCK_ADJUSTMENT 1

#define BUTTON_UP    6  // A button
#define BUTTON_DOWN  7  // B button
#define BUTTON_NONE  -1


uint32_t prevTime = 0; // Used for frames-per-second throttle

Coord outCoords[1024];
int maxOut = 1024;
int coordCount = 0;
int lastHalfMinute = -1;

int clockOffset = 0;

uint8_t code[9] = "00000000";

HOTP hotp = HOTP();
QR qr;
RTWPreferences rtc;

uint32_t lastTime = 0;
uint32_t lastButtonPress = 0;
int lastButtonPressed = 0;

#define INVERT_SWITCH 1
SwitchControl buttonUp(BUTTON_UP, 100);
SwitchControl buttonDown(BUTTON_DOWN, 100);

MatrixDisplay matrix;

void setup() {
  Serial.begin(57600);

  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  buttonUp.setup();
  buttonDown.setup();

  rtc.start();

  matrix.setup();

  // Hashed OTP
  hotp.setSecret();

  // Adjust RTC time to the build timestamp
  
  // Capture local compile time
  DateTime localBuildTime(F(__DATE__), F(__TIME__));

  const int32_t timezoneOffsetSeconds = timezoneOffsetHours * 3600;

  // Adjust local build time to UTC explicitly
  DateTime utcBuildTime(localBuildTime.unixtime() - timezoneOffsetSeconds);
  
  // Set RTC time to UTC
  rtc.adjust(utcBuildTime);

  Serial.print("RTC set to UTC build time: ");
  Serial.println(utcBuildTime.timestamp());

  clockOffset = utcBuildTime.unixtime() - rtc.now().unixtime();

  // With a clock adjustment, also reset the offset.
  rtc.resetOffset();

  buttonUp.setup();
  buttonDown.setup();
}

void drawPixel(uint8_t x, uint8_t y, bool on) {
  // Hook this to your LED matrix API
  
  matrix.drawPixel(x, y, on);
} 

void qrDrawPixel(uint8_t x, uint8_t y, bool on) {
  // The qr library places a 4 pixel padding.
  // Inverted, ensure there is a physical matte
  // dark border around the screen to aid reading.
  
  matrix.drawPixel(x - 4, y - 4, on);
} 

void drawDigits(uint32_t date_coded_year, uint8_t digits, uint8_t x, uint8_t y) {

  for (uint8_t l = digits, i = 0; l--; i++) {
    uint8_t digit = date_coded_year % 10;
    date_coded_year -= digit;
    date_coded_year /= 10;
  
    drawDigit(digit, i * -4 + x, y, drawPixel);
  }
}

void drawDigitsDoubled(uint32_t date_coded_year, uint8_t digits, uint8_t x, uint8_t y) {

  for (uint8_t l = digits, i = 0; l--; i++) {
    uint8_t digit = date_coded_year % 10;
    date_coded_year -= digit;
    date_coded_year /= 10;
  
    drawDigitDoubled(digit, i * -8 + x, y, drawPixel);
  }
}


void loop() {
  uint32_t now = millis();
  uint32_t msAgo = now - lastTime;
  
  // We don't need to update so frequently.
  //
  if (msAgo == 0) {
    return;
  }


  lastTime = now; 

  buttonUp.loop(now, msAgo);
  buttonDown.loop(now, msAgo);

  if (buttonUp.getVirtualState() && lastButtonPressed != BUTTON_UP) {

    rtc.offsetUp();

    Serial.println("Up button pressed");
    lastButtonPress = millis();
    lastButtonPressed = BUTTON_UP;
  } else if (buttonDown.getVirtualState() && lastButtonPressed != BUTTON_DOWN) {
    
    rtc.offsetDown();
    
    Serial.println("Down button pressed");
    lastButtonPress = millis();
    lastButtonPressed = BUTTON_DOWN;
  } else if (rtc.uncommittedChanges() && (millis() - lastButtonPress) > 60000) {

    Serial.println("Committing changes");
    // Identify the difference from the current eeprom
    rtc.commit();
  }

  if ((millis() - lastButtonPress) > BUTTON_DEBOUNCE_MS) {
    lastButtonPressed = BUTTON_NONE;
  }

  // Check millis first to limit frame rate
  uint32_t t;
  while(((t = micros()) - prevTime) < (1000000L / MAX_FPS));
  prevTime = t;

  // Work with RTC time to minimize clock drift
  DateTime rtcNow = rtc.now();
  unsigned long seconds = rtcNow.unixtime() / 1L;
  DateUtil dateUtil(seconds);

  unsigned long currentHalfMinute = seconds / 30L;
  
  // Only update and print when the half minute changes
  if (lastHalfMinute == seconds) {
    return;
  }
  lastHalfMinute = seconds;

  // Generate a new HOTP code
  // This includes the full 64 bit timestamp
  // in the challenge, and is compatible with 
  // the HOTP standard with 8 digit codes.
  hotp.setChallenge(currentHalfMinute);
  
  unsigned long time_hint_number = currentHalfMinute;

  // Clear the matrix
  matrix.fillScreen(false);

  
  int row = 1;

  int date_coded_year = dateUtil.getCurrentYYYY();

  drawDigits(date_coded_year, 4, WIDTH - 23, 25);

  drawDigit(DIGIT_SEP_DATE, WIDTH - 20, 25, drawPixel);

  int date_coded_date = dateUtil.getCurrentMMDD();

  drawDigits(date_coded_date / 100, 2, WIDTH - 13, 25);

  drawDigit(DIGIT_SEP_DATE, WIDTH - 10, 25, drawPixel);

  drawDigits(date_coded_date % 100, 2, WIDTH - 3, 25);
  
  int date_coded_time = dateUtil.getCurrentHHMM();

  drawDigitsDoubled(date_coded_time / 100, 2, WIDTH - 27, 4);

  drawDigitDoubled(DIGIT_SEP_TIME, WIDTH - 21, 4, drawPixel);

  drawDigitsDoubled(date_coded_time % 100, 2, WIDTH - 6, 4);

  // Convert to HHMM

  time_hint_number = (currentHalfMinute / 2) % 1440;
  time_hint_number = (time_hint_number / 60) * 100 + time_hint_number % 60;

  // We only need so many digits.
  unsigned long time_hint_divisor = 1;
  for (int i = 0; i < TIME_HINT_SECTION; ++i) {
    time_hint_divisor *= 10;
  }

  time_hint_number = time_hint_number % time_hint_divisor;


  // Write time hint to the uri string
  // This is used to identify clock skew
  for (uint8_t l = TIME_HINT_SECTION, i = 0; l--; i++) {
    uint8_t digit = time_hint_number % 10;
    time_hint_number -= digit;
    time_hint_number /= 10;
    
    uri[HOSTNAME_SECTION + l] = digit + '0'; 
  }

  // The code is what is actually checked.
  // It is chenerated using the HOTP algorithm
  // with the challenge being the full timestamp.

  // We only need so many digits.
  unsigned long code_divisor = 1;
  for (int i = 0; i < CODE_SECTION; ++i) {
    code_divisor *= 10;
  }

  unsigned long code_number = hotp.digest() % code_divisor;

  for (uint8_t l = CODE_SECTION, i = 0; l--; i++) {
    uint8_t digit = code_number % 10;
    code_number -= digit;
    code_number /= 10;
    
    uri[HOSTNAME_SECTION + TIME_HINT_SECTION + l] = digit + '0';
  }

  for (int i = 0; i < sizeof(uri); i++) {
    Serial.print((char)uri[i]);
  }
  Serial.println();

  // QR code generation is single segment.
  qr.appendBinarySegment(uri, HOSTNAME_SECTION + TIME_HINT_SECTION + CODE_SECTION);

  // Draw the QR code
  qr.toMatrix(qrDrawPixel);

  // Copy data to matrix buffers
  matrix.show();
}