#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>

#include "spring/common/log_event.hpp"
#include "spring/common/ring_buffer.hpp"

namespace euclid {
namespace spring {

template <std::size_t Capacity, std::size_t BatchSize>
class LogSink {
 static_assert(BatchSize > 0, "BatchSize must be greater than 0");
 
 public:
  explicit LogSink(
    SPSCRingBuffer<LogEvent, Capacity>& log_event_rb,
    const char* file_path)
    : log_event_rb_(log_event_rb) {
      fd_ = ::open(
        file_path,
        O_CREAT | O_WRONLY | O_TRUNC,
        0644);
    }
  ~LogSink() {
    flush_remaining();

    if (fd_ >= 0) {
      ::close(fd_);
    }
  }

  LogSink(const LogSink&) = delete;
  LogSink& operator=(const LogSink&) = delete;
  LogSink(LogSink&&) = delete;
  LogSink& operator=(LogSink&&) = delete;

  void run() {
    while (running_.load(std::memory_order_relaxed)) {
      std::size_t n = drain_batch();
      
      if (n == 0) {
        ::sched_yield();
        continue;
      }

      write_batch(batch_.data(), n);
    }

    flush_remaining();
  }

  void stop() {
    running_.store(false, std::memory_order_relaxed);
  }

 private:
  SPSCRingBuffer<LogEvent, Capacity>& log_event_rb_;
  std::array<LogEvent, BatchSize> batch_{};

  std::atomic<bool> running_{true};
  int fd_ = -1;

  std::size_t drain_batch() {
    std::size_t n = 0;
    while (n < BatchSize && log_event_rb_.try_pop(batch_[n])) {
      ++n;
    }
    return n;
  }

  void write_batch(const LogEvent* log_events, std::size_t n) {
    if (fd_ < 0 || n == 0) {
      return;
    }

    const char* data = reinterpret_cast<const char*>(log_events);
    std::size_t bytes_left = n * sizeof(LogEvent);

    while (bytes_left > 0) {
      const ssize_t written_bytes = ::write(fd_, data, bytes_left);
      
      if (written_bytes <= 0) {
        // For now, fail silently.
        // Later: count write errors.
        return;
      }

      data += written_bytes;
      bytes_left -= static_cast<std::size_t>(written_bytes);
    }
  }

  void flush_remaining() {
    while (true) {
      const std::size_t n = drain_batch();

      if (n == 0) {
        break;
      }

      write_batch(batch_.data(), n);
    }

    if (fd_ >= 0) {
      ::fsync(fd_);
    }
  }
};

} // namespace spring
} // namespace euclid