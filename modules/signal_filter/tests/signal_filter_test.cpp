// @relation(ARCH-2, scope=file, role=Test)
#include "modules/signal_filter/src/signal_filter.h"

#include <gtest/gtest.h>

TEST(SignalFilterTest, MovingAverageOverAGrowingThenFullWindow) {
  sdoctest::MovingAverageFilter filter(3);

  EXPECT_DOUBLE_EQ(filter.Push(1.0), 1.0);        // average of [1]
  EXPECT_DOUBLE_EQ(filter.Push(2.0), 1.5);        // average of [1, 2]
  EXPECT_DOUBLE_EQ(filter.Push(3.0), 2.0);        // average of [1, 2, 3]
  // Window is full (size 3): the oldest sample (1.0) is dropped.
  EXPECT_DOUBLE_EQ(filter.Push(6.0), 11.0 / 3.0); // average of [2, 3, 6]
}
