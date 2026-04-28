#pragma once

#include <cstdint>

#include "spring/common/ring_buffer.hpp"
#include "spring/market/market_event.hpp"
#include "spring/common/log_event.hpp"

namespace euclid {
namespace spring {

template <std::size_t Capacity>
class SyntheticMarketDataSource {
 public:
  explicit SyntheticMarketDataSource(
    SPSCRingBuffer<MarketEvent, Capacity>& market_event_rb,
    SPSCRingBuffer<LogEvent, Capacity>& market_event_log_rb) 
    : market_event_rb_(market_event_rb),
      market_event_log_rb_(market_event_log_rb) {}
  ~SyntheticMarketDataSource() = default;

  SyntheticMarketDataSource(const SyntheticMarketDataSource&) = delete;
  SyntheticMarketDataSource& operator=(const SyntheticMarketDataSource&) = delete;
  SyntheticMarketDataSource(SyntheticMarketDataSource&&) = delete;
  SyntheticMarketDataSource& operator=(SyntheticMarketDataSource&&) = delete;

  void run() {
    while (running_) {
      MarketEvent event = generate_next_event();
      while (!market_event_rb_.try_push(event)) {};
    }
  }

  void stop() {
    running_ = false;
  }

 private:
  SPSCRingBuffer<MarketEvent, Capacity>& market_event_rb_;
  SPSCRingBuffer<LogEvent, Capacity>& market_event_log_rb;

  bool running_ = true;

  std::uint64_t seq_no_ = 0;          
  std::uint64_t exchange_ts_ns_ = 0;  
  std::uint64_t recv_ts_ns_ = 0;
  InstrumentId instrument_id_ = 0;
  EventType event_type_ = EventType::Quote;

  Price bid_price_ = 0;
  Price ask_price_ = 0;
  Quantity bid_size_ = 0;
  Quantity ask_size_ = 0;

  Price trade_price_ = 0;
  Quantity trade_size_ = 0;
  Side aggressor_side_ = Side::Buy;

  MarketEvent generate_next_event() {
    MarketEvent event{};

    event.seq_no = ++seq_no_;
    event.exchange_ts_ns = exchange_ts_ns_;
    event.recv_ts_ns = recv_ts_ns_;
    event.instrument_id = instrument_id_;
    event.event_type = EventType::Quote;
    event.quote = Quote{
      .bid_price = bid_price_,
      .ask_price = ask_price_,
      .bid_size = bid_size_,
      .ask_size = ask_size_
    };
    
    return event;
  }
};

} // namespace spring
} // namespace euclid