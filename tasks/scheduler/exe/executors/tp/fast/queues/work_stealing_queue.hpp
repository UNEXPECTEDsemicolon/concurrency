#pragma once

#include <exe/executors/task.hpp>
#include <twist/ed/stdlike/atomic.hpp>

#include <array>
#include <span>

namespace exe::executors::tp::fast {

// Single producer / multiple consumers bounded queue
// for local tasks

template <size_t Capacity>
class WorkStealingQueue {
  struct Slot {
    twist::ed::stdlike::atomic<TaskBase*> item;
  };

 public:
  // Single producer

  bool TryPush(TaskBase* item) {
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

  // For grabbing from global queue / for stealing
  // Should always succeed
  void PushMany(std::span<TaskBase*> buffer) {
    for (auto item : buffer) {
      TryPush(item);
    }
  }

  // Multiple consumers

  // Returns nullptr if queue is empty
  TaskBase* TryPop() {
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

  // For stealing and for offloading to global queue
  // Returns number of tasks in `out_buffer`
  size_t Grab(std::span<TaskBase*> out_buffer) {
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

}  // namespace exe::executors::tp::fast
