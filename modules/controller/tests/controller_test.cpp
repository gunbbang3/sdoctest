// @relation(ARCH-6, scope=file, role=Test)
#include "modules/controller/src/controller.h"

#include <utility>

#include <gtest/gtest.h>

#include "modules/diagnostics/src/diagnostics.h"
#include "modules/logger/src/logger.h"
#include "modules/sensor_input/src/sensor_input.h"
#include "modules/signal_filter/src/signal_filter.h"

TEST(ControllerTest, RunCycleLogsOneEntryPerSampleAndFlagsOutOfRange) {
  sdoctest::SensorInput sensor({10.0, 20.0, 90.0});
  sdoctest::MovingAverageFilter filter(1);  // passthrough: avg of last 1
  sdoctest::RangeDiagnostics diagnostics(0.0, 50.0);

  sdoctest::Controller controller(std::move(sensor), std::move(filter),
                                   std::move(diagnostics), /*clamp_lo=*/0.0,
                                   /*clamp_hi=*/100.0);
  controller.RunCycle();

  const auto& entries = controller.GetLogger().Entries();
  ASSERT_EQ(entries.size(), 3u);
  // Sample 1 (10) and 2 (20) fall within the 0-50 diagnostics range.
  EXPECT_EQ(entries[0], "[INFO] signal nominal");
  EXPECT_EQ(entries[1], "[INFO] signal nominal");
  // Sample 3 (90) exceeds the 0-50 diagnostics range.
  EXPECT_EQ(entries[2], "[ERROR] signal out of range");
}
