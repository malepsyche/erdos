#pragma once

#include <thread>

#include <spring/common/event_log.hpp>
#include <spring/common/log_sink.hpp>
#include <spring/common/logger.hpp>
#include <spring/common/ring_buffer.hpp>
#include <spring/market/market_event.hpp>
#include <spring/market/synthetic_market_data_source.hpp>

namespace euclid {
namespace spring {

template <std::size_t EventCapacity, 
          std::size_t EventLogCapacity,
          std::size_t EventLogBatchSize>
class Runtime {
 public:
  explicit Runtime(
    const char* file_path
  ) : event_logger_(event_log_rb_, 0),
      event_log_sink_(event_log_rb_, file_path),
      synthetic_market_data_source_(market_event_rb_, event_logger_) {}
  ~Runtime() {
    stop();
    join();
  }

  Runtime(const Runtime&) = delete;
  Runtime& operator=(const Runtime&) = delete;
  Runtime(Runtime&&) = delete;
  Runtime& operator=(Runtime&&) = delete;

  void start() {
    event_thread_ = std::thread([this] {
      synthetic_market_data_source_.run();
    });

    event_log_sink_thread_ = std::thread([this] {
      event_log_sink_.run();
    });
  }

  void stop() {
    synthetic_market_data_source_.stop();
    event_log_sink_.stop();
  }

  void join() {
    if (event_thread_.joinable()) {
      event_thread_.join();
    }
    if (event_log_sink_thread_.joinable()) {
      event_log_sink_thread_.join();
    }
  }

 private:
  SPSCRingBuffer<MarketEvent, EventCapacity> market_event_rb_;
  SPSCRingBuffer<EventLog, EventLogCapacity> event_log_rb_;
  
  Logger<EventLogCapacity> event_logger_;
  LogSink<EventLogCapacity, EventLogBatchSize> event_log_sink_;
  SyntheticMarketDataSource<EventCapacity, EventLogCapacity> synthetic_market_data_source_;

  std::thread event_thread_;
  std::thread event_log_sink_thread_;
};

} // namespace spring
} // namespace euclid