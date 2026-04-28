#pragma once

#include <cstdint>

#include "spring/common/ring_buffer.hpp"
#include "spring/market/market_event.hpp"
#include "spring/common/log_event.hpp"
#include "spring/common/clock.hpp"

namespace euclid {
namespace spring {

template <std::size_t Capacity>
class SyntheticMarketDataSource {
 public:
  explicit SyntheticMarketDataSource(
    SPSCRingBuffer<MarketEvent, Capacity>& market_event_rb,
    SPSCRingBuffer<LogEvent, Capacity>& log_event_rb) 
    : market_event_rb_(market_event_rb),
      log_event_rb_(log_event_rb) {}
  ~SyntheticMarketDataSource() = default;

  SyntheticMarketDataSource(const SyntheticMarketDataSource&) = delete;
  SyntheticMarketDataSource& operator=(const SyntheticMarketDataSource&) = delete;
  SyntheticMarketDataSource(SyntheticMarketDataSource&&) = delete;
  SyntheticMarketDataSource& operator=(SyntheticMarketDataSource&&) = delete;

  void run() {
    while (running_) {
      MarketEvent market_event = generate_market_event();
      
      while (running_ && !market_event_rb_.try_push(market_event)) {

      }
      
      LogEvent log_event = generate_log_event(market_event); 
      log_event_rb_.try_push(log_event);
    }
  }

  void stop() {
    running_ = false;
  }

 private:
  SPSCRingBuffer<MarketEvent, Capacity>& market_event_rb_;
  SPSCRingBuffer<LogEvent, Capacity>& log_event_rb_;

  bool running_ = true;

  std::uint64_t producer_id_ = 0;
  std::uint64_t seq_no_ = 0;          
  InstrumentId instrument_id_ = 0;
  EventType event_type_ = EventType::Quote;

  Price bid_price_ = 0;
  Price ask_price_ = 0;
  Quantity bid_size_ = 0;
  Quantity ask_size_ = 0;

  Price trade_price_ = 0;
  Quantity trade_size_ = 0;
  Side aggressor_side_ = Side::Buy;

  MarketEvent generate_market_event() {
    MarketEvent market_event{};
    
    const auto now = Clock::now_ns();

    market_event.seq_no = ++seq_no_;
    market_event.exchange_ts_ns = now;
    market_event.recv_ts_ns = now;
    market_event.instrument_id = instrument_id_;
    market_event.event_type = EventType::Quote;
    market_event.quote = Quote{
      .bid_price = bid_price_,
      .ask_price = ask_price_,
      .bid_size = bid_size_,
      .ask_size = ask_size_
    };
    
    return market_event;
  }

  LogEvent generate_log_event(const MarketEvent& market_event) {
    LogEvent log_event{};

    log_event.log_ts_ns = Clock::now_ns();
    log_event.seq_no = market_event.seq_no;
    log_event.producer_id = producer_id_;
    log_event.stage = LogStage::MarketGenerated;
    log_event.market_event = market_event;

    return log_event;
  }
};

} // namespace spring
} // namespace euclid