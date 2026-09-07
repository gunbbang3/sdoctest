// @relation(ARCH-2, scope=file)
#include "modules/signal_filter/src/signal_filter.h"

namespace sdoctest {

MovingAverageFilter::MovingAverageFilter(std::size_t window_size)
    : window_size_(window_size == 0 ? 1 : window_size) {}

double MovingAverageFilter::Push(double sample) {
  window_.push_back(sample);
  sum_ += sample;
  if (window_.size() > window_size_) {
    sum_ -= window_.front();
    window_.pop_front();
  }
  return sum_ / static_cast<double>(window_.size());
}

}  // namespace sdoctest
