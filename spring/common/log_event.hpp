#pragma once

#include <cstdint>

#include "spring/market/market_event.hpp"

namespace euclid {
namespace spring {

enum class LogStage : std::uint8_t {
  MarketGenerated = 0
};

struct LogEvent {
  std::uint64_t log_ts_ns;
  std::uint64_t seq_no;
  std::uint16_t producer_id;
  LogStage stage;

  union {
    MarketEvent market_event;
  };
};

} // namespace spring
} // namespace euclid