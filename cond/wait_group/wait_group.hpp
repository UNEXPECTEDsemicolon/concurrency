#pragma once

#include <cstdlib>
#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>
#include <twist/ed/stdlike/mutex.hpp>

class WaitGroup {
 public:
  // += count
  void Add(size_t count) {
    std::lock_guard guard(mutex_);
    count_ += count;
  }

  // =- 1
  void Done() {
    std::lock_guard guard(mutex_);
    count_--;
    if (count_ == 0) {
      waiter_.notify_all();
    }
  }

  // == 0
  // One-shot
  void Wait() {
    std::unique_lock guard(mutex_);
    waiter_.wait(guard, [&]() {
      return count_ == 0;
    });
  }

 private:
  size_t count_ = 0;
  twist::ed::stdlike::mutex mutex_;
  twist::ed::stdlike::condition_variable waiter_;
};
