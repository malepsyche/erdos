#pragma once

#include <cstdint>
#include <type_traits>

#include "spring/common/clock.hpp"
#include "spring/common/ring_buffer.hpp"

#include "spring/logging/event_log.hpp"

#include "spring/market/market_event.hpp"

namespace euclid {
namespace spring {

template <std::size_t Capacity>
class Logger {
 static_assert(std::is_trivially_copyable_v<EventLog>);
 static_assert(std::is_standard_layout_v<EventLog>);
 
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
  
  template <typename Payload>
  inline void log(const Payload& payload) {
    EventLog event_log{};
    make_event_log(payload, event_log);
    
    if (!event_log_rb_.try_push(event_log)) {
      ++dropped_;
    }
  }

  std::uint64_t dropped() const {
    return dropped_;
  }

 private:
  inline void make_event_log(const MarketEvent& market_event, EventLog& event_log) const {
    event_log.log_ts_ns = Clock::now_ns();
    event_log.seq_no = market_event.seq_no;
    event_log.producer_id = producer_id_;
    event_log.stage = LogStage::MarketGenerated;
    event_log.market_event = market_event;
  }
  
 private:   
  SPSCRingBuffer<EventLog, Capacity>& event_log_rb_;

  std::uint64_t producer_id_;
  std::uint64_t dropped_ = 0;
};

} // namespace spring
} // namespace euclid