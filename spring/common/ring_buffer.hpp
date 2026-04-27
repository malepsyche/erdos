#pragma once

#include <atomic>
#include <cstddef> 
#include <new>
#include <type_traits>
#include <utility>
#include <memory>

namespace euclid {
namespace spring {

template <typename T, std::size_t Capacity>
class SPSCRingBuffer {
 static_assert(Capacity >= 2, "Capacity must be at least 2");
 static_assert((Capacity & (Capacity-1)) == 0, "Capacity must be a power of 2 for bitmasking");

 public:
  SPSCRingBuffer() = default;
  ~SPSCRingBuffer() {
    if constexpr (!std::is_trivially_destructible_v<T>) {
      std::size_t head = head_.load(std::memory_order_relaxed);
      const std::size_t tail = tail_.load(std::memory_order_relaxed);
      
      while (head != tail) {
        T* slot = ptr_at(head);
        std::destroy_at(slot);
        increment(head);
      }
    }
  };

  SPSCRingBuffer(const SPSCRingBuffer&) = delete;
  SPSCRingBuffer& operator=(const SPSCRingBuffer&) = delete;
  SPSCRingBuffer(SPSCRingBuffer&&) = delete;
  SPSCRingBuffer& operator=(SPSCRingBuffer&&) = delete;

  template <typename U>
  bool try_push(U&& value) {
    std::size_t tail = tail_.load(std::memory_order_relaxed);
    std::size_t next = increment(tail);

    if (next == head_.load(std::memory_order_acquire)) {
      return false;
    }

    T* slot = ptr_at(tail);
    std::construct_at(slot, std::forward<U>(value));

    tail_.store(next, std::memory_order_release);
    return true;
  }

  bool try_pop(T& out) {
    std::size_t head = head_.load(std::memory_order_relaxed);
    
    if (head == tail_.load(std::memory_order_acquire)) {
      return false;
    }

    T* slot = ptr_at(head);
    out = std::move(*slot);
    std::destroy_at(slot);

    head_.store(increment(head), std::memory_order_release);
    return true;
  }

  bool empty() const {
    return head_.load(std::memory_order_relaxed) == 
           tail_.load(std::memory_order_relaxed);
  }

  bool full() const {
    const std::size_t tail = tail_.load(std::memory_order_relaxed);
    return increment(tail) == head_.load(std::memory_order_relaxed);
  }

 private:
  static constexpr std::size_t increment(std::size_t i) noexcept {
    return (i+1) & mask_;
  }

  T* ptr_at(std::size_t i) noexcept {
    return std::launder(reinterpret_cast<T*>(&storage_[i]));
  }

  const T* ptr_at(std::size_t i) const noexcept {
    return std::launder(reinterpret_cast<const T*>(&storage_[i]));
  }

 private:
  static constexpr std::size_t mask_ = Capacity - 1;
  static constexpr std::size_t cacheline_size_ = 64;

  alignas(T) std::byte storage_[Capacity][sizeof(T)];
  alignas(cacheline_size_) std::atomic<std::size_t> head_{0};
  alignas(cacheline_size_) std::atomic<std::size_t> tail_{0};
};

} // namespace spring
} // namespace euclid