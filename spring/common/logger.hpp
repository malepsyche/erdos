#pragma once

#include <cstdint>

#include "spring/common/clock.hpp"
#include "spring/common/log_event.hpp"
#include "spring/common/ring_buffer.hpp"

#include "spring/market/market_event.hpp"

namespace euclid {
namespace spring {

template <std::size_t Capacity>
class Logger {
 public:
  explicit Logger(
    SPSCRingBuffer<LogEvent, Capacity>& log_event_rb)
    : log_event_rb_(log_event_rb) {}
  ~Logger() = default;
    
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(Logger&&) = delete;

  
  inline void push_log_event(const MarketEvent& market_event) {
    LogEvent log_event{};

    log_event.log_ts_ns = Clock::now_ns();
    log_event.seq_no = market_event.seq_no;
    log_event.producer_id = producer_id_;
    log_event.stage = LogStage::MarketGenerated;
    log_event.market_event = market_event;

    if (!log_event_rb_.try_push(log_event)) {
      ++dropped_;
    }
  }

  std::uint64_t dropped() const {
    return dropped_;
  }
  
 private:   
  SPSCRingBuffer<LogEvent, Capacity>& log_event_rb_;

  std::uint64_t producer_id_;
  std::uint64_t dropped_ = 0;
};

} // namespace spring
} // namespace euclid