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

#ifndef HOTP_h
#define HOTP_h

#include "SHA1.hpp"
#include "Base32.hpp"

#define SECRET_SECTION            33
#define COUNTER_SECTION           15
#define SHA1_DIGEST_LENGTH        20
#define HMAC_KEY_LENGTH           64
#define CHALLENGE_LENGTH          8

class HOTP {
public:
  HOTP();

  void setSecret();
  void setChallenge(uint64_t counterAt);
  uint32_t digest();
  
  
private:

  uint16_t readOffset;
  uint8_t msg[8];
  size_t msgLength;
  uint8_t key[64];
  size_t keyLength;
};


HOTP::HOTP()
{
  

}

void HOTP::setSecret()
{
  uint16_t messageLen = SECRET_SECTION - 1;
  
  uint16_t buffer = 0;
  uint8_t sigBits = 0;
  uint16_t wroteBytes = 0;

  
  for (uint16_t i = 0; i < messageLen; i++) {
    buffer <<= 5;
    buffer |= fromBase32(secretBase32[i]);
    sigBits += 5;
    if (sigBits >= 8) {
      key[wroteBytes++] = buffer >> (sigBits - 8);
      sigBits -= 8;
    }
  }
  
  keyLength = wroteBytes;
}


void HOTP::setChallenge(uint64_t counterAt)
{
  uint64_t counter = counterAt;

  for (uint8_t l = 8; l--;) {
    uint8_t writeValue = counter % 256;
    msg[l] = writeValue;
    counter -= writeValue;
    counter = counter >> 8;
    if (counter == 0) continue;
  }

  msgLength = 8;
}

uint32_t HOTP::digest()
{ 
  uint8_t oKeyPad[HMAC_KEY_LENGTH] = {0};
  uint8_t iKeyPad[HMAC_KEY_LENGTH] = {0};

  for (int i = 0; i < 64; ++i) {
      if (i < keyLength) {
          oKeyPad[i] = 0x5c ^ key[i];
          iKeyPad[i] = 0x36 ^ key[i];
      } else {
          oKeyPad[i] = 0x5c;
          iKeyPad[i] = 0x36;
      }
  }

  SHA1 innerSha;
  innerSha.addBytes(iKeyPad, 64);
  innerSha.addBytes(msg, msgLength);
  uint8_t innerHash[SHA1_DIGEST_LENGTH];
  innerSha.digest(innerHash);

  SHA1 outerSha;
  outerSha.addBytes(oKeyPad, 64);
  outerSha.addBytes(innerHash, SHA1_DIGEST_LENGTH);
  uint8_t hmacResult[SHA1_DIGEST_LENGTH];
  outerSha.digest(hmacResult);

  uint8_t offset = hmacResult[SHA1_DIGEST_LENGTH-1] & 0xf;
  
  uint32_t result = 0;

  result |= (uint32_t)(hmacResult[offset] & 0x7f) << 24;
  result |= (uint32_t)(hmacResult[offset + 1]) << 16;
  result |= (uint32_t)(hmacResult[offset + 2]) << 8;
  result |= (uint32_t)(hmacResult[offset + 3]);

  return result;
}

#endif // HOTP_h
