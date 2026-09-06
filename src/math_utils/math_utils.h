// @relation(ARCH-3, scope=file)
#pragma once

namespace sdoctest {

// Returns the sum of two values.
double Add(double a, double b);

// Clamps `value` into the closed interval [lo, hi].
double Clamp(double value, double lo, double hi);

}  // namespace sdoctest
