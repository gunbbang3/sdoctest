// @relation(ARCH-2, scope=file, role=Test)
#include "modules/signal_filter/src/signal_filter.h"

#include <iostream>

#include "tests/test_util.h"

int main() {
  using sdoctest::test::ExpectNear;

  sdoctest::MovingAverageFilter filter(3);

  ExpectNear(filter.Push(1.0), 1.0, 1e-9, "average of [1]");
  ExpectNear(filter.Push(2.0), 1.5, 1e-9, "average of [1, 2]");
  ExpectNear(filter.Push(3.0), 2.0, 1e-9, "average of [1, 2, 3]");
  // Window is full (size 3): the oldest sample (1.0) is dropped.
  ExpectNear(filter.Push(6.0), 11.0 / 3.0, 1e-9, "average of [2, 3, 6]");

  std::cout << "OK" << std::endl;
  return 0;
}
