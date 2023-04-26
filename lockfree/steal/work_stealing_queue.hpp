#pragma once

#include <array>
#include <cstddef>
#include <span>
#include <fmt/core.h>

#include <twist/ed/stdlike/atomic.hpp>

// Single-Producer / Multi-Consumer Bounded Ring Buffer

template <typename T, size_t Capacity>
class WorkStealingQueue {
  struct Slot {
    twist::ed::stdlike::atomic<T*> item = nullptr;
  };

 public:
  bool TryPush(T* item) {
    while (true) {
      auto cur_end = end_.load();
      if ((cur_end + kRealCapacity - begin_.load()) % kRealCapacity ==
              Capacity ||
          buffer_[cur_end].item.load()) {
        return false;
      }
      auto next = (cur_end + 1) % (kRealCapacity);
      if (end_.compare_exchange_strong(cur_end, next)) {
        buffer_[cur_end].item.store(item);
        return true;
      }
    }
  }

  // Returns nullptr if queue is empty
  T* TryPop() {
    while (true) {
      auto cur_begin = begin_.load();
      if (cur_begin == end_.load() || !buffer_[cur_begin].item.load()) {
        return nullptr;
      }
      auto next = (cur_begin + 1) % kRealCapacity;
      if (begin_.compare_exchange_strong(cur_begin, next)) {
        return buffer_[cur_begin].item.exchange(nullptr);
      }
    }
  }

  // Returns number of tasks
  size_t Grab(std::span<T*> out_buffer) {
    for (size_t i = 0; i < out_buffer.size(); ++i) {
      out_buffer[i] = TryPop();
      if (out_buffer[i] == nullptr) {
        return i;
      }
    }
    return out_buffer.size();
  }

 private:
  inline static const auto kRealCapacity = Capacity + 1;

  twist::ed::stdlike::atomic<size_t> begin_ = 0;
  twist::ed::stdlike::atomic<size_t> end_ = 0;
  std::array<Slot, kRealCapacity> buffer_;
};
