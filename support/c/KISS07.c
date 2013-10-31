#include <stdio.h>
#include <stdint.h>
#include "mash.h"
 
// George Marsaglia, 2007-06-23
//http://groups.google.com/group/comp.lang.fortran/msg/6edb8ad6ec5421a5
 
const double norm32 = 2.3283064365386963e-10;   // 2^-32
 
static uint32_t x = 123456789;
static uint32_t y = 362436069;
static uint32_t z = 21288629;
static uint32_t w = 14921776;
static uint32_t c = 0;
 
static inline double kiss07(void) {
  uint32_t t;
 
  x += 545925293;
 
  y ^= y << 13;
  y ^= y >> 17;
  y ^= y << 5;
 
  t = z + w + c;
  z = w;
  c = t >> 31;
  w = t & 0x7fffffff;
 
  return (x + y + w) * norm32;  // 2^-32;
}
 
int main(int argc, unsigned char *argv[]) {
  double buffer[256];
  int i;
 
  for (i = 1; i < argc; i++) {
    x ^= (uint32_t) (mash(argv[i]) * 0x100000000ULL);   // 2^32
    y ^= (uint32_t) (mash(argv[i]) * 0x100000000ULL);   // 2^32
    z ^= (uint32_t) (mash(argv[i]) * 0x100000000ULL);   // 2^32
    w ^= (uint32_t) (mash(argv[i]) * 0x100000000ULL);   // 2^32
  }
  if (y == 0) {
    y = 1;
  }
  c ^= z >> 31;
  z &= 0x7fffffff;
  if ((z % 7559) == 0) {
    z++;
  }
  w &= 0x7fffffff;
  if ((w % 7559) == 0) {
    w++;
  }
 
  while (1) {
    for (i = 0; i < 256; i++) {
      buffer[i] = kiss07();
    }
    fwrite(buffer, sizeof buffer, 1, stdout);
  }
}