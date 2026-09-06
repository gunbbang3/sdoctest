// @relation(ARCH-6, scope=file)
#pragma once

#include "src/diagnostics/diagnostics.h"
#include "src/logger/logger.h"
#include "src/sensor_input/sensor_input.h"
#include "src/signal_filter/signal_filter.h"

namespace sdoctest {

// Integrates signal acquisition, filtering, range diagnostics, and logging
// into a single control cycle.
class Controller {
 public:
  Controller(SensorInput sensor, MovingAverageFilter filter,
             RangeDiagnostics diagnostics, double clamp_lo, double clamp_hi);

  // Runs one full cycle: reads every raw sample, filters it, clamps it,
  // checks it for an out-of-range fault, and logs the outcome.
  void RunCycle();

  const Logger& GetLogger() const;

 private:
  SensorInput sensor_;
  MovingAverageFilter filter_;
  RangeDiagnostics diagnostics_;
  double clamp_lo_;
  double clamp_hi_;
  Logger logger_;
};

}  // namespace sdoctest
