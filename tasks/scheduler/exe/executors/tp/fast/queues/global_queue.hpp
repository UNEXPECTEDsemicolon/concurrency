#pragma once

#include <exe/executors/task.hpp>

#include <mutex>
#include <wheels/intrusive/forward_list.hpp>

#include <span>
#include "twist/ed/stdlike/mutex.hpp"

namespace exe::executors::tp::fast {

// Unbounded queue shared between workers

class GlobalQueue {
 public:
  void Push(TaskBase* item) {
    std::lock_guard guard(mutex_);
    queue_.PushBack(item);
  }

  void Offload(std::span<TaskBase*> buffer) {
    wheels::IntrusiveForwardList<TaskBase> input;
    for (auto item : buffer) {
      input.PushBack(item);
    }
    std::lock_guard guard(mutex_);
    queue_.Append(input);
  }

  // Returns nullptr if queue is empty
  TaskBase* TryPop() {
    std::lock_guard guard(mutex_);
    return queue_.PopFront();
  }

  // Returns number of items in `out_buffer`
  size_t Grab(std::span<TaskBase*> out_buffer, size_t workers) {
    size_t res = 0;
    std::lock_guard guard(mutex_);
    while (!queue_.IsEmpty() && res < workers) {
      out_buffer[res++] = queue_.PopFront();
    }
    return res;
  }

 private:
  wheels::IntrusiveForwardList<TaskBase> queue_;
  twist::ed::stdlike::mutex mutex_;
};

}  // namespace exe::executors::tp::fast
