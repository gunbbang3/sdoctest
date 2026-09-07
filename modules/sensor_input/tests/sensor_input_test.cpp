// @relation(ARCH-1, scope=file, role=Test)
#include "modules/sensor_input/src/sensor_input.h"

#include <gtest/gtest.h>

TEST(SensorInputTest, SampleCountReportsAllSamples) {
  sdoctest::SensorInput sensor({1.0, 2.5, 3.0});

  EXPECT_EQ(sensor.SampleCount(), 3u);
}

TEST(SensorInputTest, ReadRawReturnsEachSampleInOrder) {
  sdoctest::SensorInput sensor({1.0, 2.5, 3.0});

  EXPECT_DOUBLE_EQ(sensor.ReadRaw(0), 1.0);
  EXPECT_DOUBLE_EQ(sensor.ReadRaw(1), 2.5);
  EXPECT_DOUBLE_EQ(sensor.ReadRaw(2), 3.0);
}
