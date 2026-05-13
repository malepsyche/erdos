#pragma once

#include <cstdint>
#include <type_traits>

#include "spring/market/market_event.hpp"

namespace euclid {
namespace spring {

enum struct LogStage : std::uint8_t {
  MarketGenerated = 0
};

struct EventLog {
  std::uint64_t log_ts_ns;
  std::uint64_t seq_no;
  std::uint16_t producer_id;
  LogStage stage;

  union {
    MarketEvent market_event;
  };
};

static_assert(std::is_trivially_copyable_v<EventLog>);
static_assert(std::is_standard_layout_v<EventLog>);

} // namespace spring
} // namespace euclid