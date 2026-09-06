// @relation(ARCH-5, scope=file, role=Test)
#include "src/logger/logger.h"

#include <iostream>

#include "tests/test_util.h"

int main() {
  using sdoctest::test::ExpectTrue;

  sdoctest::Logger logger;
  logger.Log(sdoctest::LogSeverity::kInfo, "startup");
  logger.Log(sdoctest::LogSeverity::kError, "signal out of range");

  ExpectTrue(logger.Entries().size() == 2, "both entries recorded");
  ExpectTrue(logger.Entries()[0] == "[INFO] startup",
             "info entry formatted with severity label");
  ExpectTrue(logger.Entries()[1] == "[ERROR] signal out of range",
             "error entry formatted with severity label");

  std::cout << "OK" << std::endl;
  return 0;
}
