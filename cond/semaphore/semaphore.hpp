#pragma once

#include <mutex>
#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>

#include <cstdlib>

class Semaphore {
 public:
  explicit Semaphore(size_t tokens)
      : tokens_(tokens) {
  }

  void Acquire() {
    std::unique_lock guard(mutex_);
    cv_.wait(guard, [&]() {
      return tokens_ != 0;
    });
    tokens_--;
  }

  void Release() {
    std::lock_guard quard(mutex_);
    tokens_++;
    cv_.notify_one();
  }

 private:
  twist::ed::stdlike::condition_variable cv_;
  twist::ed::stdlike::mutex mutex_;
  size_t tokens_;
};
