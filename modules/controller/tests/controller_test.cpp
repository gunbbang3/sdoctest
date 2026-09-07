// @relation(ARCH-6, scope=file, role=Test)
#include "modules/controller/src/controller.h"

#include <iostream>
#include <utility>

#include "modules/diagnostics/src/diagnostics.h"
#include "modules/logger/src/logger.h"
#include "modules/sensor_input/src/sensor_input.h"
#include "modules/signal_filter/src/signal_filter.h"
#include "tests/test_util.h"

int main() {
  using sdoctest::test::ExpectTrue;

  sdoctest::SensorInput sensor({10.0, 20.0, 90.0});
  sdoctest::MovingAverageFilter filter(1);  // passthrough: avg of last 1
  sdoctest::RangeDiagnostics diagnostics(0.0, 50.0);

  sdoctest::Controller controller(std::move(sensor), std::move(filter),
                                   std::move(diagnostics), /*clamp_lo=*/0.0,
                                   /*clamp_hi=*/100.0);
  controller.RunCycle();

  const auto& entries = controller.GetLogger().Entries();
  ExpectTrue(entries.size() == 3, "one log entry per sample");
  ExpectTrue(entries[0] == "[INFO] signal nominal", "sample 1 (10) nominal");
  ExpectTrue(entries[1] == "[INFO] signal nominal", "sample 2 (20) nominal");
  ExpectTrue(entries[2] == "[ERROR] signal out of range",
             "sample 3 (90) exceeds the 0-50 diagnostics range");

  std::cout << "OK" << std::endl;
  return 0;
}
