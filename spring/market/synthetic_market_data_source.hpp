#pragma once

#include "spring/common/ring_buffer.hpp"
#include "spring/market/market_event.hpp"

namespace euclid {
namespace spring {

template <std::size_t Capacity>
class SyntheticMarketDataSource {
 public:
  explicit SyntheticMarketDataSource(SPSCRingBuffer<MarketEvent, Capacity>& ring_buffer) : ring_buffer_(ring_buffer) {}
  ~SyntheticMarketDataSource() = default;

  SyntheticMarketDataSource(const SyntheticMarketDataSource&) = delete;
  SyntheticMarketDataSource& operator=(const SyntheticMarketDataSource&) = delete;
  SyntheticMarketDataSource(SyntheticMarketDataSource&&) = delete;
  SyntheticMarketDataSource& operator=(SyntheticMarketDataSource&&) = delete;

  void run() {
    while (running_) {
      MarketEvent event = generate_next_event();
      ring_buffer_.try_push(event);
    }
  }

  void stop() {
    running_ = false;
  }

 private:
  SPSCRingBuffer<MarketEvent, Capacity>& ring_buffer_;

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