#pragma once

#include <atomic>
#include <chrono>
#include <cstddef>
#include <fcntl.h>
#include <string>
#include <thread>
#include <unistd.h>

namespace euclid {
namespace prism {

template <typename EventLog, typename EventLogFormatter>  
class LogDumper {  
 public:
  explicit LogDumper(
    const char* log_bin_path,
    const char* log_txt_path
  ) {
    log_bin_fd_ = ::open(log_bin_path, O_RDONLY);
    log_txt_fd_ = ::open(
      log_txt_path,
      O_CREAT | O_WRONLY | O_TRUNC,
      0644);
  }
  ~LogDumper() {
    if (log_bin_fd_ >= 0) {
      ::close(log_bin_fd_);
    }
    if (log_txt_fd_ >= 0) {
      ::close(log_txt_fd_);
    }
  };

  LogDumper(const LogDumper&) = delete;
  LogDumper& operator=(const LogDumper&) = delete;
  LogDumper(LogDumper&&) = delete;
  LogDumper& operator=(LogDumper&&) = delete;

  void run() {
    if (!assert_fds()) {
      return;
    }

    EventLog event_log{};
    std::string event_log_str;
    event_log_str.reserve(256);

    while (
      running_.load(std::memory_order_relaxed) ||
      can_read_full_record()
    ) {
      if (!can_read_full_record()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        continue;
      }

      if (!read_one(event_log)) {
        break;
      }

      event_log_str.clear();
      event_log_formatter_.format_event_log(event_log, event_log_str);
      
      if (!write_one(event_log_str)) {
        break;
      }
    }
  }

  void stop() {
    running_.store(false, std::memory_order_relaxed);
  }

 private:
  std::atomic<bool> running_{true};
  int log_bin_fd_ = -1;
  int log_txt_fd_ = -1;

  EventLogFormatter event_log_formatter_;
  
  inline bool assert_fds() const {
    return (log_bin_fd_ >= 0) && (log_txt_fd_ >= 0);
  }

  bool can_read_full_record() {
    off_t cur = ::lseek(log_bin_fd_, 0, SEEK_CUR);
    off_t end = ::lseek(log_bin_fd_, 0, SEEK_END);

    if (cur < 0 || end < 0) {
      return false;
    }

    ::lseek(log_bin_fd_, cur, SEEK_SET);
    
    return static_cast<std::size_t>(end-cur) >= sizeof(EventLog);
  }

  bool read_one(EventLog& event_log) const {
    char* data = reinterpret_cast<char*>(&event_log);
    std::size_t bytes_left = sizeof(EventLog);

    while (bytes_left > 0) {
      const ssize_t read_bytes = ::read(log_bin_fd_, data, bytes_left);
      
      if (read_bytes <= 0) {
        // For now, fail silently.
        // Later: count write errors.
        return false;
      }

      data += read_bytes;
      bytes_left -= static_cast<std::size_t>(read_bytes);
    }

    return true;
  }

  bool write_one(const std::string& event_log_str) const {
    const char* data = event_log_str.data();
    std::size_t bytes_left = event_log_str.size();
    
    while (bytes_left > 0) {      
      ssize_t written_bytes = ::write(log_txt_fd_, data, bytes_left);
      
      if (written_bytes <= 0) {
        // For now, fail silently.
        // Later: count write errors.
        return false;
      }
      
      data += written_bytes;
      bytes_left -= static_cast<std::size_t>(written_bytes);
    }

    return true;
  }

};

} // namespace prism
} // namespace euclid