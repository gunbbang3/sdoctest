// @relation(ARCH-4, scope=file)
#pragma once

namespace sdoctest {

enum class FaultCode {
  kNone,
  kOutOfRange,
};

// Checks whether a (filtered) signal value stays within an allowed range.
class RangeDiagnostics {
 public:
  RangeDiagnostics(double lo, double hi);

  // Returns FaultCode::kOutOfRange if `value` falls outside [lo, hi],
  // FaultCode::kNone otherwise.
  FaultCode Check(double value) const;

 private:
  double lo_;
  double hi_;
};

}  // namespace sdoctest
