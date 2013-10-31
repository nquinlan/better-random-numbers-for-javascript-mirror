static inline double mash(unsigned char *x) {
  static uint32_t n = 0xefc8249d;
  double h;
 
  while (*x) {
    n += *x++;
    h = 0.02519603282416938 * n;
    n = h;
    h -= n;
    h *= n;
    n = h;
    h -= n;
    n += h * 0x100000000ULL;
  }
  return n * 2.3283064365386963e-10; // 2^-32
}