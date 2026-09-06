#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

namespace sdoctest::test {

// Minimal assertion helpers so module tests stay dependency-free (no
// googletest) but still fail loudly and reliably in any build mode,
// unlike `assert()`, which optimized (-c opt) builds can strip.

inline void ExpectTrue(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << "FAILED: " << message << std::endl;
    std::exit(1);
  }
}

inline void ExpectNear(double actual, double expected, double tolerance,
                        const std::string& message) {
  ExpectTrue(std::fabs(actual - expected) <= tolerance,
             message + " (actual=" + std::to_string(actual) +
                 ", expected=" + std::to_string(expected) + ")");
}

}  // namespace sdoctest::test
