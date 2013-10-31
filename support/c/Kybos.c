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
 
static double s[8];
static int k = 0;
 
static inline double kybos() {
  double r;
 
  k = s[k] * 8.0;
  r = s[k];
  s[k] -= alea();
  if (s[k] < 0) {
    s[k] += 1;
  }
  return r;
}
 
int main(int argc, unsigned char *argv[]) {
  double buffer[256];
  int i, j;
 
  s0 = mash(" ");
  s1 = mash(" ");
  s2 = mash(" ");
  for (j = 0; j < 8; j++) {
    s[j] = mash(" ");
  }
 
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
    for (j = 0; j < 8; j++) {
      s[j] -= mash(argv[i]);
      if (s[j] < 0.0) {
        s[j] += 1.0;
      }
    }
  }
 
  while (1) {
    for (i = 0; i < 256; i++) {
      buffer[i] = kybos();
    }
    fwrite(buffer, sizeof buffer, 1, stdout);
  }
}