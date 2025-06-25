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


#ifndef SwitchControl_h
#define SwitchControl_h

#ifdef INVERT_SWITCH
  #define SWITCH_HELD HIGH
#else
  #define SWITCH_HELD LOW
#endif

class SwitchControl {
public:
  SwitchControl(uint8_t switchPin, uint32_t msToStateChange);
  void loop(uint32_t now, uint32_t msAgo);
  void setup();

  uint8_t getVirtualState();
  
private:
  uint8_t _switchPin;
  uint8_t _virtualState;
  uint32_t _switchMsHeld;
  uint32_t _msToStateChange;  
  uint32_t _lastLoopAtMillis;
};


inline SwitchControl::SwitchControl(uint8_t switchPin, uint32_t msToStateChange)
{
  _switchPin = switchPin;
  _msToStateChange = msToStateChange;
  _virtualState = LOW;
  _switchMsHeld = 0;
  _lastLoopAtMillis = 0;
}

inline void SwitchControl::setup()
{
  pinMode(_switchPin, INPUT_PULLUP);
}

inline void SwitchControl::loop(uint32_t now, uint32_t msAgo)
{
  
  uint8_t physState = digitalRead(_switchPin);
  
  if (physState == SWITCH_HELD) {
    if (_switchMsHeld < _msToStateChange) {
      _switchMsHeld += msAgo;
    } else {
      _virtualState = HIGH;
    }
  } else {
    if (_switchMsHeld > msAgo) {
      _switchMsHeld -= msAgo;
    } else {
      _virtualState = LOW;
    }
  }

  _lastLoopAtMillis = now;
  
}

inline uint8_t SwitchControl::getVirtualState()
{
  if (_virtualState == HIGH) return HIGH;
  else return LOW;
}

#endif // SwitchControl_h
