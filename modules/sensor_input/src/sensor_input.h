// @relation(ARCH-1, scope=file)
#pragma once

#include <cstddef>
#include <vector>

namespace sdoctest {

// Provides raw, unfiltered readings from a (simulated) sensor.
class SensorInput {
 public:
  explicit SensorInput(std::vector<double> raw_samples);

  // Number of raw samples available.
  std::size_t SampleCount() const;

  // Returns the raw sample at `index`. `index` must be < SampleCount().
  double ReadRaw(std::size_t index) const;

 private:
  std::vector<double> raw_samples_;
};

}  // namespace sdoctest
