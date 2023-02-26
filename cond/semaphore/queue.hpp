#pragma once

#include "tagged_semaphore.hpp"

#include <deque>

// Bounded Blocking Multi-Producer/Multi-Consumer (MPMC) Queue

template <typename T>
class BoundedBlockingQueue {
 public:
  explicit BoundedBlockingQueue(size_t capacity)
      : free_space_(capacity) {
  }

  void Put(T value) {
    auto data_token = free_space_.Acquire();
    auto lock_token = locker_.Acquire();
    buffer_.emplace_back(std::move(value));
    locker_.Release(std::move(lock_token));
    used_space_.Release(std::move(data_token));
  }

  T Take() {
    auto data_token = used_space_.Acquire();
    auto lock_token = locker_.Acquire();
    auto result = std::move(buffer_.front());
    buffer_.pop_front();
    locker_.Release(std::move(lock_token));
    free_space_.Release(std::move(data_token));
    return result;
  }

 private:
  // Tags
  struct DataTag {};
  struct LockTag {};

 private:
  TaggedSemaphore<LockTag> locker_{1};
  TaggedSemaphore<DataTag> used_space_{0};
  TaggedSemaphore<DataTag> free_space_;
  std::deque<T> buffer_;
};
