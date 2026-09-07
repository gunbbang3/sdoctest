// @relation(ARCH-4, scope=file)
#include "modules/diagnostics/src/diagnostics.h"

namespace sdoctest {

RangeDiagnostics::RangeDiagnostics(double lo, double hi) : lo_(lo), hi_(hi) {}

FaultCode RangeDiagnostics::Check(double value) const {
  if (value < lo_ || value > hi_) {
    return FaultCode::kOutOfRange;
  }
  return FaultCode::kNone;
}

}  // namespace sdoctest
