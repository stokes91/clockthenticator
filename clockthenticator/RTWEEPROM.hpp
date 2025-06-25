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


#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include <EEPROM.h>

#include <RTClib.h>

#ifndef RTWEEPROM_h
#define RTWEEPROM_h



class RTWEEPROM {
 public:
   RTWEEPROM();
 
   void setup();
   
   void offsetUp();
   void offsetDown();
   DateTime now();
   void commit();
   void adjust(DateTime dt);
   bool needsAdjustment();
 private:
 
  RTC_PCF8523 rtc;

  long offsetEEPROM = 0;
  long offsetEEPROMCommitted = 0;

};
 

RTWEEPROM::RTWEEPROM()
{
  

}

RTWEEPROM::~RTWEEPROM()
{
  

}

void RTWEEPROM::setup() {

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  rtc.start();


  offsetEEPROM = EEPROM.read(0);
  offsetEEPROMCommitted = offsetEEPROM;
}

bool RTWEEPROM::needsAdjustment() {
  return rtc.lostPower() || rtc.initialized();
}

void RTWEEPROM::adjust(DateTime dt) {
  rtc.adjust(dt);
}

void RTWEEPROM::offsetUp() {
  offsetEEPROM += 60;

  DateTime now = rtc.now() + offsetEEPROM;


  unsigned long seconds = now.unixtime() / 1L;

  // Round to the nearest minute
  // Discard spare seconds
  offsetEEPROM -= (seconds % 60);
  
}

void RTWEEPROM::offsetDown() {
  offsetEEPROM -= 60;

  DateTime now = rtc.now() + offsetEEPROM;


  unsigned long seconds = now.unixtime() / 1L;

  // Round to the nearest minute
  // Discard spare seconds
  offsetEEPROM -= (seconds % 60);
}

DateTime RTWEEPROM::now() {
  return rtc.now() + offsetEEPROM;
}

void RTWEEPROM::commit() {

  int offsetEEPROMDifference = offsetEEPROM - offsetEEPROMCommitted;
  Serial.print("Offset difference: ");
  Serial.println(offsetEEPROMDifference);
  int initialEEPROMValue = EEPROM.read(0);
  Serial.print("Initial EEPROM value: ");
  Serial.println(initialEEPROMValue);

  EEPROM.write(0, offsetEEPROM);
  Serial.println("Offset saved to EEPROM");
  offsetEEPROMCommitted = offsetEEPROM;

  int finalEEPROMValue = EEPROM.read(0);
  Serial.print("Final EEPROM value: ");
  Serial.println(finalEEPROMValue);

}
#endif