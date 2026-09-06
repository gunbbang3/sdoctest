// @relation(ARCH-3, scope=file)
#include "src/math_utils/math_utils.h"

namespace sdoctest {

double Add(double a, double b) { return a + b; }

double Clamp(double value, double lo, double hi) {
  if (value < lo) return lo;
  if (value > hi) return hi;
  return value;
}

}  // namespace sdoctest
