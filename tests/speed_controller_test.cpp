// @relation(ARCH-7, scope=file, role=Test)
//
// Unlike the other module tests, ARCH-7 links to src/speed_controller/*
// via a doc-side `TYPE: File` relation (see docs/architecture.sdoc)
// rather than an in-file marker, because that code is Simulink-generated
// and must not be hand-edited. This test file itself is hand-written, so
// it carries the usual source marker.
extern "C" {
#include "src/speed_controller/speed_controller.h"
}

#include <iostream>

#include "tests/test_util.h"

int main() {
  using sdoctest::test::ExpectNear;

  speed_controller_initialize();

  // Step 1: proportional term dominates, integrator state starts at 0.
  speed_controller_U.Error = 10.0;
  speed_controller_step();
  ExpectNear(speed_controller_Y.Command, 20.0, 1e-9,
             "first step: Kp * error, integrator still 0");

  // Step 2: integrator has accumulated Ki * error * SampleTime = 0.5.
  speed_controller_step();
  ExpectNear(speed_controller_Y.Command, 20.5, 1e-9,
             "second step includes the accumulated integrator term");

  // A large error should saturate the output at the upper limit.
  speed_controller_U.Error = 1000.0;
  speed_controller_step();
  ExpectNear(speed_controller_Y.Command, 100.0, 1e-9,
             "large error saturates at the upper limit");

  std::cout << "OK" << std::endl;
  return 0;
}
