#include <stdio.h>
#include <stdint.h>
#include "mash.h"
 
const double norm32 = 2.3283064365386963e-10;   // 2^-32
 
static double s0, s1, s2;
static int c = 1;
 
static inline double alea() {
  double t = 2091639.0 * s0 + c * norm32;       // 2^-32
  s0 = s1;
  s1 = s2;
  return s2 = t - (c = t);
}
 
int main(int argc, unsigned char *argv[]) {
  double buffer[256];
  int i;
 
  s0 = mash(" ");
  s1 = mash(" ");
  s2 = mash(" ");
 
  for (i = 1; i < argc; i++) {
    s0 -= mash(argv[i]);
    if (s0 < 0.0) {
      s0 += 1.0;
    }
    s1 -= mash(argv[i]);
    if (s1 < 0.0) {
      s1 += 1.0;
    }
    s2 -= mash(argv[i]);
    if (s2 < 0.0) {
      s2 += 1.0;
    }
  }
 
  while (1) {
    for (i = 0; i < 256; i++) {
      buffer[i] = alea();
    }
    fwrite(buffer, sizeof buffer, 1, stdout);
  }
}