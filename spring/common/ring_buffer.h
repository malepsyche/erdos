#pragma once

#include <atomic>
#include <cstddef> 

namespace euclid {
namespace spring {

template <typename T, std::size_t Capacity>
class SPSCRingBuffer {
  static_assert(Capacity >= 2, "Capacity must be at least 2");
  static_assert((Capacity & (Capacity-1)) == 0, "Capacity must be a power of 2 for bitmasking");

  public:
    SPSCRingBuffer() = default;
    ~SPSCRingBuffer() {};

  private:
    static constexpr std::size_t increment(std::size_t i) noexcept {
      return (i+1) & mask_;
    }

    T* at_ptr(std::size_t i) noexcept {
      return std::launder(reinterpret_cast<T*>(&storage_[i]));
    }

    const T* at_ptr(std::size_t i) const noexcept {
      return std::launder(reinterpret_cast<const T*>(&storage_[i]));
    }

  private:
    static constexpr std::size_t mask_ = Capacity - 1;
    static constexpr std::size_t cacheline_size_ = 64;

    alignas(T) std::byte storage_[Capacity][sizeof(T)];
    alignas(cacheline_size_) std::atomic<std::size_t> head{0};
    alignas(cacheline_size_) std::atomic<std::size_t> tail{0};
};

} // namespace spring
} // namespace euclid