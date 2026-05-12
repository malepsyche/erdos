#pragma once

#include <thread>

#include "prism/log_dumper.hpp"

namespace euclid {
namespace prism {

class Runtime {
 public:
  Runtime(
    const char* log_bin_path,
    const char* log_txt_path
  ) : log_dumper_(log_bin_path, log_txt_path) {}
  ~Runtime() {
    stop();
    join();
  }

  void start() {
    log_dumper_thread_ = std::thread([this] {
      log_dumper_.run();
    });
  }

  void stop() {
    log_dumper_.stop();
  }

  void join() {
    if (log_dumper_thread_.joinable()) {
      log_dumper_thread_.join();
    }
  }

  Runtime(const Runtime&) = delete;
  Runtime& operator=(const Runtime&) = delete;
  Runtime(Runtime&&) = delete;
  Runtime& operator=(Runtime&&) = delete;

 private:
  LogDumper log_dumper_;

  std::thread log_dumper_thread_;
};

} // namespace prism
} // namespace euclid