#pragma once

#include <cstdint>

namespace euclid {
namespace spring {

using InstrumentId = std::uint32_t;
using Price        = std::int64_t;   
using Quantity     = std::int64_t;

enum class EventType : std::uint8_t {
    Quote = 0,
    Trade = 1
};

enum class Side : std::uint8_t {
    Bid = 0,
    Ask = 1,
    Unknown = 2
};

struct MarketEvent {
  std::uint64_t seq_no;          
  std::uint64_t exchange_ts_ns;  
  std::uint64_t recv_ts_ns;      

  InstrumentId instrument_id;
  EventType type;

  Price bid_price;
  Price ask_price;
  Quantity bid_size;
  Quantity ask_size;

  Price trade_price;
  Quantity trade_size;
  Side aggressor_side;
};

} // namespace spring
} // namespace euclid