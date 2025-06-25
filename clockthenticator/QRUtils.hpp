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

#ifndef QR_UTILS_H
#define QR_UTILS_H

inline int GetMaskBit(int mask, int y, int x) {
  switch(mask) {
    case 0: return ((x + y) % 2 == 0) ? 1 : 0;
    case 1: return (y % 2 == 0) ? 1 : 0;
    case 2: return (x % 3 == 0) ? 1 : 0;
    case 3: return ((x + y) % 3 == 0) ? 1 : 0;
    case 4: return (((x / 3) + (y / 2)) % 2 == 0) ? 1 : 0;
    case 5: return (((x * y) % 2) + ((x * y) % 3) == 0) ? 1 : 0;
    case 6: return ((((x * y) % 2) + ((x * y) % 3)) % 2 == 0) ? 1 : 0;
    case 7: return ((((x + y) % 2) + ((x * y) % 3)) % 2 == 0) ? 1 : 0;
    case 8: return 1;
    default: return 0; // error case
  }
}

#endif // QR_UTILS_H