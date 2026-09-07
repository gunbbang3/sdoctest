// @relation(ARCH-5, scope=file, role=Test)
#include "modules/logger/src/logger.h"

#include <gtest/gtest.h>

TEST(LoggerTest, EntriesAreFormattedWithSeverityLabel) {
  sdoctest::Logger logger;
  logger.Log(sdoctest::LogSeverity::kInfo, "startup");
  logger.Log(sdoctest::LogSeverity::kError, "signal out of range");

  ASSERT_EQ(logger.Entries().size(), 2u);
  EXPECT_EQ(logger.Entries()[0], "[INFO] startup");
  EXPECT_EQ(logger.Entries()[1], "[ERROR] signal out of range");
}
