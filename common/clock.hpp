#pragma once

#include <chrono>
#include <cstdint>

namespace euclid {
namespace spring {

class Clock {
 public:
  static inline std::uint64_t now_ns() {
    using clock = std::chrono::steady_clock;

    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        clock::now().time_since_epoch())
        .count();
  }
};

} // namespace spring
} // namespace euclid