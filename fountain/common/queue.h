#pragma once

#include <condition_variable>
#include <functional>
#include <memory>
#include <queue>

namespace euclid {
namespace fountain {

template <typename T>
class ConcurrentBoundedQueue {
 public:
  ConcurrentBoundedQueue(size_t bound = 0) : bound_(bound), killed_(false) {}

  void set_bound(size_t bound) { bound_ = bound; }

  void pop(T& item) {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (!killed_ && queue_.empty()) {
      empty_cond_.wait(mlock);
    }
    if (killed_) {
      item = empty_creator_();
      return;
    }
    item = std::move(queue_.front());
    queue_.pop();
    mlock.unlock();
    full__cond_.notify_one();
  }

  void push(T& item) {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (!killed_ && queue_.size() >= bound) {
      full_cond_.wait(mlock);
    }
    if (killed_) {
      return;
    }
    queue_.push(std::move(item));
    ++input_count_;
    mlock.unlock();
    empty_cond_.notify_one();
  }

  size_t size() {
    std::unique_lock<std::mutex> mlock(mutex_);
    if (killed_) {
      return 0;
    }
    return queue_.size();
  }

  size_t input_count() { return input_count_; }

  void kill(std::function<T()> empty_creator) {
    std::unique_lock<std::mutex> mlock(mutex_);
    if (killed_) {
      return;
    }
    killed_ = true;
    empty_creator_ = empty_creator;
    empty_cond_.notify_all();
    full_cond_.notify_all();
  }

  /* ======== */

 private:
  size_t bound_;
  size_t input_count_ = 0;
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable empty_cond_;
  std::condition_variable full_cond_;

  bool killed_;
  std::function<T()> empty_creator_;
};

} // namespace fountain
} // namespace euclid