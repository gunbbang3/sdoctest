// @relation(ARCH-3, scope=file, role=Test)
#include "modules/math_utils/src/math_utils.h"

#include <gtest/gtest.h>

TEST(MathUtilsTest, AddSumsTwoValues) {
  EXPECT_DOUBLE_EQ(sdoctest::Add(2.0, 3.0), 5.0);
  EXPECT_DOUBLE_EQ(sdoctest::Add(-2.0, 2.0), 0.0);
}

TEST(MathUtilsTest, ClampWithinRangeIsUnchanged) {
  EXPECT_DOUBLE_EQ(sdoctest::Clamp(5.0, 0.0, 10.0), 5.0);
}

TEST(MathUtilsTest, ClampBelowRangeIsRaisedToLowerBound) {
  EXPECT_DOUBLE_EQ(sdoctest::Clamp(-1.0, 0.0, 10.0), 0.0);
}

TEST(MathUtilsTest, ClampAboveRangeIsLoweredToUpperBound) {
  EXPECT_DOUBLE_EQ(sdoctest::Clamp(11.0, 0.0, 10.0), 10.0);
}
