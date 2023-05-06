#pragma once

#include <exe/executors/task.hpp>
#include <twist/ed/stdlike/atomic.hpp>

#include <array>
#include <span>
#include "fmt/core.h"

namespace exe::executors::tp::fast {

// Single producer / multiple consumers bounded queue
// for local tasks

template <size_t Capacity>
class WorkStealingQueue {
  struct Slot {
    twist::ed::stdlike::atomic<TaskBase*> item = nullptr;
  };

 public:
  // Single producer

  bool TryPush(TaskBase* item) {
    auto begin = begin_.load();
    auto end = end_.load();
    if ((end - begin) == Capacity) {
      return false;
    }
    buffer_[end % Capacity].item.store(item);
    end_.fetch_add(1);
    return true;
  }

  // For grabbing from global queue / for stealing
  // Should always succeed
  void PushMany(std::span<TaskBase*> buffer) {
    for (auto item : buffer) {
      while (!TryPush(item)) {
        fmt::println("here");
      }
    }
  }

  // Multiple consumers

  // Returns nullptr if queue is empty
  TaskBase* TryPop() {
    while (true) {
      auto begin = begin_.load();
      auto end = end_.load();
      if (begin == end) {
        return nullptr;
      }
      auto res = buffer_[begin % Capacity].item.load();
      if (begin_.compare_exchange_strong(begin, begin + 1)) {
        return res;
      }
    }
  }

  // For stealing and for offloading to global queue
  // Returns number of tasks in `out_buffer`
  size_t Grab(std::span<TaskBase*> out_buffer) {
    for (size_t i = 0; i < out_buffer.size(); ++i) {
      if (!(out_buffer[i] = TryPop())) {
        return i;
      }
    }
    return out_buffer.size();
  }

  inline size_t Size() const {
    return end_.load() - begin_.load();
  }

 private:
  // inline static const auto kRealCapacity = Capacity + 1;

  alignas(std::max_align_t) twist::ed::stdlike::atomic<size_t> begin_ = 0;
  alignas(std::max_align_t) twist::ed::stdlike::atomic<size_t> end_ = 0;
  alignas(std::max_align_t) std::array<Slot, Capacity> buffer_;
};

}  // namespace exe::executors::tp::fast
