#pragma once

#include <thread>

#include "common/ring_buffer.hpp"

#include "logging/log_sink.hpp"
#include "logging/logger.hpp"

#include "spring/logging/event_log.hpp"

#include "spring/market/market_event.hpp"
#include "spring/market/synthetic_market_data_source.hpp"

namespace euclid {
namespace spring {

template <std::size_t EventCapacity, 
          std::size_t EventLogCapacity,
          std::size_t EventLogBatchSize>
class SyntheticMarketDataRuntime {
 public:
  explicit SyntheticMarketDataRuntime(
    const char* log_bin_path
  ) : logger_(event_log_rb_, 0),
      log_sink_(event_log_rb_, log_bin_path),
      source_(event_rb_, logger_) {}
  ~SyntheticMarketDataRuntime() {
    stop();
    join();
  }

  SyntheticMarketDataRuntime(const SyntheticMarketDataRuntime&) = delete;
  SyntheticMarketDataRuntime& operator=(const SyntheticMarketDataRuntime&) = delete;
  SyntheticMarketDataRuntime(SyntheticMarketDataRuntime&&) = delete;
  SyntheticMarketDataRuntime& operator=(SyntheticMarketDataRuntime&&) = delete;

  void start() {
    source_thread_ = std::thread([this] {
      source_.run();
    });

    log_sink_thread_ = std::thread([this] {
      log_sink_.run();
    });
  }

  void stop() {
    source_.stop();
    log_sink_.stop();
  }

  void join() {
    if (source_thread_.joinable()) {
      source_thread_.join();
    }
    if (log_sink_thread_.joinable()) {
      log_sink_thread_.join();
    }
  }

 private:
  SPSCRingBuffer<MarketEvent, EventCapacity> event_rb_;
  SPSCRingBuffer<EventLog<MarketEvent>, EventLogCapacity> event_log_rb_;
  
  Logger<EventLog<MarketEvent>, EventLogCapacity> logger_;
  LogSink<EventLog<MarketEvent>, EventLogCapacity, EventLogBatchSize> log_sink_;
  
  SyntheticMarketDataSource<EventCapacity, EventLogCapacity> source_;

  std::thread source_thread_;
  std::thread log_sink_thread_;
};

} // namespace spring
} // namespace euclid