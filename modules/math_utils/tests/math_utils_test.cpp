// @relation(ARCH-3, scope=file, role=Test)
#include "modules/math_utils/src/math_utils.h"

#include <iostream>

#include "tests/test_util.h"

int main() {
  using sdoctest::test::ExpectNear;

  ExpectNear(sdoctest::Add(2.0, 3.0), 5.0, 1e-9, "Add(2, 3) == 5");
  ExpectNear(sdoctest::Add(-2.0, 2.0), 0.0, 1e-9, "Add(-2, 2) == 0");

  ExpectNear(sdoctest::Clamp(5.0, 0.0, 10.0), 5.0, 1e-9, "Clamp within range");
  ExpectNear(sdoctest::Clamp(-1.0, 0.0, 10.0), 0.0, 1e-9, "Clamp below range");
  ExpectNear(sdoctest::Clamp(11.0, 0.0, 10.0), 10.0, 1e-9, "Clamp above range");

  std::cout << "OK" << std::endl;
  return 0;
}
