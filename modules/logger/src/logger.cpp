// @relation(ARCH-5, scope=file)
#include "modules/logger/src/logger.h"

namespace sdoctest {

namespace {

const char* SeverityLabel(LogSeverity severity) {
  switch (severity) {
    case LogSeverity::kInfo:
      return "INFO";
    case LogSeverity::kWarning:
      return "WARNING";
    case LogSeverity::kError:
      return "ERROR";
  }
  return "UNKNOWN";
}

}  // namespace

void Logger::Log(LogSeverity severity, const std::string& message) {
  entries_.push_back(std::string("[") + SeverityLabel(severity) + "] " +
                      message);
}

const std::vector<std::string>& Logger::Entries() const { return entries_; }

}  // namespace sdoctest
