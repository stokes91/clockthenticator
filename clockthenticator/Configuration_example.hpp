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

#define CLOCK_ADJUSTMENT 1

/*
  Compilation automatically inserts the build time in the format: __DATE__ and __TIME__
  But does not account for timezone offset, so we need to adjust it manually
  The following timezone offset shifts your timezone into UTC
*/
const int timezoneOffsetHours = -4; // Computer compiling this code's timezone offset

// Timezones for display

// America/New_York
// DSTRule dstStartRule = {3, 2, -5, SundayModifier::Second};
// DSTRule dstEndRule = {11, 2, -4, SundayModifier::First};

// America/Chicago
// DSTRule dstStartRule = {3, 2, -6, SundayModifier::Second};
// DSTRule dstEndRule = {11, 2, -5, SundayModifier::First};

// America/Denver
// DSTRule dstStartRule = {3, 2, -7, SundayModifier::Second};
// DSTRule dstEndRule = {11, 2, -6, SundayModifier::First};

// America/Phoenix
// DSTRule dstStartRule = {3, 2, -7, SundayModifier::Second};
// DSTRule dstEndRule = {11, 2, -7, SundayModifier::First};   // MST year round

// America/Los_Angeles
// DSTRule dstStartRule = {3, 2, -8, SundayModifier::Second};
// DSTRule dstEndRule = {11, 2, -7, SundayModifier::First};

// America/Anchorage
// DSTRule dstStartRule = {3, 2, -9, SundayModifier::Second};
// DSTRule dstEndRule = {11, 2, -8, SundayModifier::First};

// Pacific/Honolulu
// DSTRule dstStartRule = {3, 2, -10, SundayModifier::Second};
// DSTRule dstEndRule = {11, 2, -10, SundayModifier::First};  // No DST Observation

// Europe/Helsinki
// DSTRule dstStartRule = {3, 3, +2, SundayModifier::Last};   // EU date changes
// DSTRule dstEndRule = {10, 4, +3, SundayModifier::Last};


/*
  This is the secret key for the HOTP algorithm
  It should be a random string of 32 characters
  of base 32 (A-Z, 2-7)
*/
#define SECRET_SECTION            32
uint8_t secretBase32[SECRET_SECTION + 1] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";


/*
  The useful range of code section is up to 9.

  HOTP code returns a 32 bit value, 
  [0, 2,147,483,647]
  The code section is the least significant 
  digits of this code returned.

  The most the three sections can total to is 32
  If the number of characters exceeds 32, 
  the qr will not show data, but a checkerboard pattern

  The time hint is used to help the server get a hint to
  time skew. There can be creative uses to this later.
  */
#define CODE_SECTION              9
#define TIME_HINT_SECTION         2
#define HOSTNAME_SECTION          21
uint8_t uri[HOSTNAME_SECTION + TIME_HINT_SECTION + CODE_SECTION + 1] = "https://example.com/?";
