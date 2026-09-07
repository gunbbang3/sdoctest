// @relation(ARCH-6, scope=file)
#pragma once

#include "modules/diagnostics/src/diagnostics.h"
#include "modules/logger/src/logger.h"
#include "modules/sensor_input/src/sensor_input.h"
#include "modules/signal_filter/src/signal_filter.h"

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
