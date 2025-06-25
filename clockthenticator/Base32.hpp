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

#ifndef BASE32_h
#define BASE32_h

uint8_t getBase32(uint8_t j);
uint8_t fromBase32(uint8_t j);

uint8_t getBase32(uint8_t j) {
  if (j >= 0 && j <= 26) {
    j += 'A';
  } else if (j >= 27) {
    j += '2' - 26;
  }
  return j & 0x1f;
}

uint8_t fromBase32(uint8_t j) {
  if (j >= 'A' && j <= 'Z') {
    j -= 'A';
  } else if (j >= '2' && j <= '7') {
    j -= '2' - 26;
  }
  return j & 0x1f;
}


#endif // BASE32_h
