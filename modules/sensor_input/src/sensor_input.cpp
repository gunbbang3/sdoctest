// @relation(ARCH-1, scope=file)
#include "modules/sensor_input/src/sensor_input.h"

#include <utility>

namespace sdoctest {

SensorInput::SensorInput(std::vector<double> raw_samples)
    : raw_samples_(std::move(raw_samples)) {}

std::size_t SensorInput::SampleCount() const { return raw_samples_.size(); }

double SensorInput::ReadRaw(std::size_t index) const {
  return raw_samples_.at(index);
}

}  // namespace sdoctest
