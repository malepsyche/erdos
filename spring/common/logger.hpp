#pragma once

#include <cstdint>

#include "spring/common/clock.hpp"
#include "spring/common/event_log.hpp"
#include "spring/common/ring_buffer.hpp"

#include "spring/market/market_event.hpp"

namespace euclid {
namespace spring {

template <std::size_t Capacity>
class Logger {
 public:
  explicit Logger(
    SPSCRingBuffer<EventLog, Capacity>& event_log_rb,
    std::uint64_t producer_id)
    : event_log_rb_(event_log_rb),
      producer_id_(producer_id) {}
  ~Logger() = default;
    
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(Logger&&) = delete;
  
  inline void push_event_log(const MarketEvent& market_event) {
    EventLog event_log{};

    event_log.log_ts_ns = Clock::now_ns();
    event_log.seq_no = market_event.seq_no;
    event_log.producer_id = producer_id_;
    event_log.stage = LogStage::MarketGenerated;
    event_log.market_event = market_event;

    if (!event_log_rb_.try_push(event_log)) {
      ++dropped_;
    }
  }

  std::uint64_t dropped() const {
    return dropped_;
  }
  
 private:   
  SPSCRingBuffer<EventLog, Capacity>& event_log_rb_;

  std::uint64_t producer_id_;
  std::uint64_t dropped_ = 0;
};

} // namespace spring
} // namespace euclid