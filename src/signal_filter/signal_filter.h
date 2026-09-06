// @relation(ARCH-2, scope=file)
#pragma once

#include <cstddef>
#include <deque>

namespace sdoctest {

// Smooths a noisy input stream with a simple moving average.
class MovingAverageFilter {
 public:
  // `window_size` must be >= 1.
  explicit MovingAverageFilter(std::size_t window_size);

  // Pushes a new raw sample and returns the current moving average over
  // the last `window_size` samples (or fewer, until the window fills up).
  double Push(double sample);

 private:
  std::size_t window_size_;
  std::deque<double> window_;
  double sum_ = 0.0;
};

}  // namespace sdoctest
