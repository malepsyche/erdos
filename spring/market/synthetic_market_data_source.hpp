#pragma once

#include <atomic>
#include <cstdint>

#include "spring/common/clock.hpp"
#include "spring/common/logger.hpp"
#include "spring/common/ring_buffer.hpp"

#include "spring/market/market_event.hpp"

namespace euclid {
namespace spring {

template <std::size_t EventBufferCapacity, std::size_t LogBufferCapacity>
class SyntheticMarketDataSource {
 public:
  explicit SyntheticMarketDataSource(
    SPSCRingBuffer<MarketEvent, EventBufferCapacity>& market_event_rb,
    Logger<LogBufferCapacity>& logger) 
    : market_event_rb_(market_event_rb),
      logger_(logger) {}
  ~SyntheticMarketDataSource() = default;

  SyntheticMarketDataSource(const SyntheticMarketDataSource&) = delete;
  SyntheticMarketDataSource& operator=(const SyntheticMarketDataSource&) = delete;
  SyntheticMarketDataSource(SyntheticMarketDataSource&&) = delete;
  SyntheticMarketDataSource& operator=(SyntheticMarketDataSource&&) = delete;

  void run() {
    while (running_.load(std::memory_order_relaxed)) {
      MarketEvent market_event = generate_market_event();
      
      while (
        running_.load(std::memory_order_relaxed) && 
        !market_event_rb_.try_push(market_event)
      ) {

      }

      logger_.push_log_event(market_event);      
    }
  }

  void stop() {
    running_.store(false, std::memory_order_relaxed);
  }

 private:
  SPSCRingBuffer<MarketEvent, EventBufferCapacity>& market_event_rb_;
  Logger<LogBufferCapacity>& logger_;
  
  std::atomic<bool> running_{true};

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
};

} // namespace spring
} // namespace euclid