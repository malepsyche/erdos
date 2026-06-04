#pragma once

#include <cstdint>

namespace euclid {
namespace spring {

enum struct EventStage : std::uint8_t {
  MarketGenerated = 0,
};

template <typename EventPayload>
struct EventLog {
  std::uint64_t log_ts_ns;
  std::uint64_t seq_no;
  std::uint16_t producer_id;

  EventPayload event_payload;
  EventStage event_stage;
};

} // namespace spring
} // namespace euclid