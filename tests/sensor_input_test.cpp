// @relation(ARCH-1, scope=file, role=Test)
#include "src/sensor_input/sensor_input.h"

#include <iostream>

#include "tests/test_util.h"

int main() {
  using sdoctest::test::ExpectNear;
  using sdoctest::test::ExpectTrue;

  sdoctest::SensorInput sensor({1.0, 2.5, 3.0});

  ExpectTrue(sensor.SampleCount() == 3, "SampleCount reports all samples");
  ExpectNear(sensor.ReadRaw(0), 1.0, 1e-9, "ReadRaw(0)");
  ExpectNear(sensor.ReadRaw(1), 2.5, 1e-9, "ReadRaw(1)");
  ExpectNear(sensor.ReadRaw(2), 3.0, 1e-9, "ReadRaw(2)");

  std::cout << "OK" << std::endl;
  return 0;
}
