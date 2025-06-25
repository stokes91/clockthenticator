/*
 * Copyright 2022 Alexander D Stokes
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include <Preferences.h>

Preferences prefs;
#include <RTClib.h>

#ifndef RTWPreferences_h
#define RTWPreferences_h



class RTWPreferences {
public:
   RTWPreferences();
 
   bool begin();
   void start();
   
   void resetOffset();
   void offsetUp();
   void offsetDown();
   DateTime now();
   void commit();
   void adjust(DateTime dt);
   bool needsAdjustment();
   bool uncommittedChanges();
private:
 
  RTC_PCF8523 rtc;

  long offsetEEPROM = 0;
  long offsetEEPROMCommitted = 0;

};
 

RTWPreferences::RTWPreferences()
{
  

}

bool RTWPreferences::begin() {
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  prefs.begin("rtc", false);

  offsetEEPROM = prefs.getLong("offsetEEPROM");
  offsetEEPROMCommitted = offsetEEPROM;

  return true;
}

void RTWPreferences::start() {

  rtc.start();

}

bool RTWPreferences::needsAdjustment() {
  return rtc.lostPower() || !rtc.initialized();
}

bool RTWPreferences::uncommittedChanges() {
  return offsetEEPROM != offsetEEPROMCommitted;
}

void RTWPreferences::adjust(DateTime dt) {
  rtc.adjust(dt);
}

void RTWPreferences::resetOffset() {
  offsetEEPROM = 0;
  commit();
}

void RTWPreferences::offsetUp() {
  offsetEEPROM += 60;

  DateTime now = rtc.now() + offsetEEPROM;


  unsigned long seconds = now.unixtime() / 1L;

  // Round to the nearest minute
  // Discard spare seconds
  offsetEEPROM -= (seconds % 60);
  
}

void RTWPreferences::offsetDown() {
  offsetEEPROM -= 60;

  DateTime now = rtc.now() + offsetEEPROM;


  unsigned long seconds = now.unixtime() / 1L;

  // Round to the nearest minute
  // Discard spare seconds
  offsetEEPROM -= (seconds % 60);
}

DateTime RTWPreferences::now() {
  return rtc.now() + offsetEEPROM;
}

void RTWPreferences::commit() {

  int offsetEEPROMDifference = offsetEEPROM - offsetEEPROMCommitted;
  Serial.print("Offset difference: ");
  Serial.println(offsetEEPROMDifference);
  int initialEEPROMValue = prefs.getLong("offsetEEPROM");
  Serial.print("Initial EEPROM value: ");
  Serial.println(initialEEPROMValue);

  prefs.putLong("offsetEEPROM", offsetEEPROM);
  Serial.println("Offset saved to EEPROM");
  offsetEEPROMCommitted = offsetEEPROM;

  int finalEEPROMValue = prefs.getLong("offsetEEPROM");
  Serial.print("Final EEPROM value: ");
  Serial.println(finalEEPROMValue);

}
#endif