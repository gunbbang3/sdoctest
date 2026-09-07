// @relation(ARCH-6, scope=file)
#include "modules/controller/src/controller.h"

#include <utility>

#include "modules/math_utils/src/math_utils.h"

namespace sdoctest {

Controller::Controller(SensorInput sensor, MovingAverageFilter filter,
                        RangeDiagnostics diagnostics, double clamp_lo,
                        double clamp_hi)
    : sensor_(std::move(sensor)),
      filter_(std::move(filter)),
      diagnostics_(std::move(diagnostics)),
      clamp_lo_(clamp_lo),
      clamp_hi_(clamp_hi) {}

void Controller::RunCycle() {
  for (std::size_t i = 0; i < sensor_.SampleCount(); ++i) {
    const double raw = sensor_.ReadRaw(i);
    const double filtered = filter_.Push(raw);
    const double clamped = Clamp(filtered, clamp_lo_, clamp_hi_);

    if (diagnostics_.Check(clamped) == FaultCode::kOutOfRange) {
      logger_.Log(LogSeverity::kError, "signal out of range");
    } else {
      logger_.Log(LogSeverity::kInfo, "signal nominal");
    }
  }
}

const Logger& Controller::GetLogger() const { return logger_; }

}  // namespace sdoctest
