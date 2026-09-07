// @relation(ARCH-4, scope=file, role=Test)
#include "modules/diagnostics/src/diagnostics.h"

#include <gtest/gtest.h>

TEST(DiagnosticsTest, ValueWithinRangeReportsNoFault) {
  sdoctest::RangeDiagnostics diagnostics(0.0, 100.0);

  EXPECT_EQ(diagnostics.Check(50.0), sdoctest::FaultCode::kNone);
}

TEST(DiagnosticsTest, ValueBelowRangeReportsAFault) {
  sdoctest::RangeDiagnostics diagnostics(0.0, 100.0);

  EXPECT_EQ(diagnostics.Check(-1.0), sdoctest::FaultCode::kOutOfRange);
}

TEST(DiagnosticsTest, ValueAboveRangeReportsAFault) {
  sdoctest::RangeDiagnostics diagnostics(0.0, 100.0);

  EXPECT_EQ(diagnostics.Check(101.0), sdoctest::FaultCode::kOutOfRange);
}
