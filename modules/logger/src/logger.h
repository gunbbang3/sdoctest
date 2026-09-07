// @relation(ARCH-5, scope=file)
#pragma once

#include <string>
#include <vector>

namespace sdoctest {

enum class LogSeverity {
  kInfo,
  kWarning,
  kError,
};

// Records severity-tagged log entries in memory so tests (and, in a real
// deployment, a diagnostic backend) can inspect them.
class Logger {
 public:
  void Log(LogSeverity severity, const std::string& message);

  const std::vector<std::string>& Entries() const;

 private:
  std::vector<std::string> entries_;
};

}  // namespace sdoctest
