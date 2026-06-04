#pragma once

#include <thread>

#include "spring/market/market_event.hpp"
#include "spring/logging/event_log.hpp"
#include "spring/logging/event_log_formatter.hpp"

#include "logging/log_dumper.hpp"

namespace euclid {
namespace prism {

class Runtime {
 public:
  Runtime(
    const char* log_bin_path,
    const char* log_txt_path
  ) : market_log_dumper_(log_bin_path, log_txt_path) {}
  ~Runtime() {
    stop();
    join();
  }

  void start() {
    market_log_dumper_thread_ = std::thread([this] {
      market_log_dumper_.run();
    });
  }

  void stop() {
    market_log_dumper_.stop();
  }

  void join() {
    if (market_log_dumper_thread_.joinable()) {
      market_log_dumper_thread_.join();
    }
  }

  Runtime(const Runtime&) = delete;
  Runtime& operator=(const Runtime&) = delete;
  Runtime(Runtime&&) = delete;
  Runtime& operator=(Runtime&&) = delete;

 private:
  using MarketEventLog = euclid::spring::EventLog<euclid::spring::MarketEvent>;
  using MarketEventLogFormatter = euclid::spring::EventLogFormatter<MarketEventLog>;
  using MarketLogDumper = LogDumper<MarketEventLog, MarketEventLogFormatter>;
  MarketLogDumper market_log_dumper_;
  std::thread market_log_dumper_thread_;
};

} // namespace prism
} // namespace euclid