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
