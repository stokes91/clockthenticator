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

#include "DateUtil.hpp"

#include "Configuration.hpp"
// Configuration.hpp contains the following:
// timezoneOffsetHours
// hostnameSectionLength
// secretBase32 which is needed for HOTP

#include "HOTP.hpp"
#include "Coord.hpp"
#include "QR.hpp"
#include "DotMatrixDisplay.hpp"

/* LED Matrix */
#include "MatrixDisplay_LED.hpp"

#include "RTWPreferences.hpp"

#include <Wire.h>
#include <RTClib.h>

#define MAX_FPS 1 // Maximum redraw rate, frames/second

uint32_t prevTime = 0; // Used for frames-per-second throttle

Coord outCoords[1024];
int maxOut = 1024;
int coordCount = 0;
int lastHalfMinute = -1;

int clockOffset = 0;

int notMaskId = -1;
uint8_t code[9] = "00000000";

HOTP hotp = HOTP();
QR qr;
RTWPreferences rtc;

uint32_t lastTime = 0;

MatrixDisplay matrix;

bool indicatorAm = true;
bool indicatorPm = false;

void setup() {
  Serial.begin(57600);

  
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  rtc.start();

  matrix.setup();

  // Hashed OTP
  hotp.setSecret();

  // Adjust RTC time to the build timestamp
  if (CLOCK_ADJUSTMENT) {
      
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
  }
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


void loop() {

  uint32_t now = millis();
  uint32_t msAgo = now - lastTime;
  
  // We don't need to update so frequently.
  //
  if (msAgo == 0) {
    return;
  }


  lastTime = now; 


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
  if (lastHalfMinute == currentHalfMinute) {
    return;
  }
  lastHalfMinute = currentHalfMinute;

  // Generate a new HOTP code
  // This includes the full 64 bit timestamp
  // in the challenge, and is compatible with 
  // the HOTP standard with 8 digit codes.
  hotp.setChallenge(currentHalfMinute);
  
  unsigned long time_hint_number = currentHalfMinute;

  int row = 1;

  // Adjusted local time
  DSTBounds usBounds = updateDSTBounds(dateUtil.getCurrentYYYY(), dstStartRule, dstEndRule);
  int localTZOffset = getUTCOffsetForRegion(seconds, dstStartRule.utcOffset, usBounds);

  Serial.print("Local offset: ");
  Serial.println(localTZOffset);

  long localTimeSeconds = seconds + localTZOffset * 3600;
  DateUtil localDateUtil(localTimeSeconds);

  int date_coded_year = localDateUtil.getCurrentYYYY();

  int date_coded_time = localDateUtil.getCurrentHHMM();

  // Clear the matrix
  matrix.fillScreen(false);

  if ((date_coded_time > 0 && date_coded_time < 600) || 
      (date_coded_time > 1800 && date_coded_time < 2400)) {
    matrix.setFgColor(32, 16, 3);
  } else {
    matrix.setFgColor(255, 255, 255);
  }

  move(29, 16);
  writeText(AIRPORT_CODE, AIRPORT_CODE_LENGTH, drawPixel);
  
  if (date_coded_time >= 1200) {
    date_coded_time -= 1200;
    indicatorPm = true;
    indicatorAm = false;
  } else {
    indicatorPm = false;
    indicatorAm = true;
  }
  if (date_coded_time < 60) {
    date_coded_time += 1200;
  }


  int date_coded_date = localDateUtil.getCurrentMMDD();

// ── Group 1: full date (YYYY–MM–DD) ──


  move(29, 25);

  writeDigits(date_coded_year, 4, drawPixel);
  writeText("-", 1, drawPixel);
  writeDigits(date_coded_date/100, 2, drawPixel);
  writeText("-", 1, drawPixel);
  writeDigits(date_coded_date%100, 2, drawPixel);


// ── Group 2: AM/PM indicator ──

  move(57, 16);

  writeText(indicatorAm ? "A" : "P", 1, drawPixel);
  writeText("m", 1, drawPixel);


// ── Group 3: doubled‐size time (HH:MM) ──

  move(29, 0);

  writeDigitsDoubled(date_coded_time/100, 2, drawPixel);
  writeTextDoubled(":", 1, drawPixel);
  writeDigitsDoubled(date_coded_time%100, 2, drawPixel);

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

  // Don't display codes if the clock was adjusted to the build time.
  // Return the clock adjustment to 0 to display codes in Configuration.hpp
  if (!CLOCK_ADJUSTMENT) {
    // QR code generation is single segment.
    qr.appendBinarySegment(uri, HOSTNAME_SECTION + TIME_HINT_SECTION + CODE_SECTION);

    // Draw the QR code
    matrix.setFgColor(255, 255, 255);
    matrix.setBgColor(0, 0, 0);
    qr.toMatrix(qrDrawPixel);

    // Ensure the mask gets changed each time
    notMaskId = qr.getBestMaskId();
    qr.setNotMaskId(notMaskId);
  }

  // Copy data to matrix buffers
  matrix.show();
}