// @relation(ARCH-4, scope=file, role=Test)
#include "src/diagnostics/diagnostics.h"

#include <iostream>

#include "tests/test_util.h"

int main() {
  using sdoctest::test::ExpectTrue;

  sdoctest::RangeDiagnostics diagnostics(0.0, 100.0);

  ExpectTrue(diagnostics.Check(50.0) == sdoctest::FaultCode::kNone,
             "value within range reports no fault");
  ExpectTrue(diagnostics.Check(-1.0) == sdoctest::FaultCode::kOutOfRange,
             "value below range reports a fault");
  ExpectTrue(diagnostics.Check(101.0) == sdoctest::FaultCode::kOutOfRange,
             "value above range reports a fault");

  std::cout << "OK" << std::endl;
  return 0;
}
