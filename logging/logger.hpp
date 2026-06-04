#pragma once

#include <cstdint>
#include <type_traits>

#include "common/clock.hpp"
#include "common/ring_buffer.hpp"

namespace euclid {
namespace spring {

template <typename EventLog,
          std::size_t Capacity>
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
  
  inline void log(EventLog& event_log) {
    build_event_log(event_log);
    
    if (!event_log_rb_.try_push(event_log)) {
      ++dropped_;
    }
  }

  std::uint64_t dropped() const {
    return dropped_;
  }

 private:
  inline void build_event_log(EventLog& event_log) const {
    event_log.log_ts_ns = Clock::now_ns();
    event_log.producer_id = producer_id_;
  }
  
 private:   
  SPSCRingBuffer<EventLog, Capacity>& event_log_rb_;

  std::uint64_t producer_id_;
  std::uint64_t dropped_ = 0;
};

} // namespace spring
} // namespace euclid