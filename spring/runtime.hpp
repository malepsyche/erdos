#pragma once

#include <thread>

#include "spring/market/synthetic_market_data_runtime.hpp"

namespace euclid {
namespace spring {

template <std::size_t EventCapacity, 
          std::size_t EventLogCapacity,
          std::size_t EventLogBatchSize>
class Runtime {
 public:
  explicit Runtime(
    const char* log_bin_path
  ) : synthetic_market_data_runtime_(log_bin_path) {}
  ~Runtime() {
    stop();
    join();
  }

  Runtime(const Runtime&) = delete;
  Runtime& operator=(const Runtime&) = delete;
  Runtime(Runtime&&) = delete;
  Runtime& operator=(Runtime&&) = delete;

  void start() {
    synthetic_market_data_runtime_.start();
  }

  void stop() {
    synthetic_market_data_runtime_.stop();
  }

  void join() {
    synthetic_market_data_runtime_.join();
  }

 private:
  SyntheticMarketDataRuntime<EventCapacity, EventLogCapacity, EventLogBatchSize> synthetic_market_data_runtime_;
};

} // namespace spring
} // namespace euclid