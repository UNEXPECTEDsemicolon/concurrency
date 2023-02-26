#pragma once

#include <mutex>
#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>

#include <cstdlib>

class CyclicBarrier {
 public:
  explicit CyclicBarrier(size_t participants)
      : participants_(participants) {
  }

  void ArriveAndWait() {
    std::unique_lock guard(mutex_);
    waiter_for_flush_.wait(guard, [&]() {
      return counter_ != participants_;
    });
    counter_++;
    if (counter_ == participants_) {
      waiter_for_prev_.notify_all();
    } else {
      waiters_++;
      waiter_for_prev_.wait(guard, [&]() {
        return counter_ == participants_;
      });
      waiters_--;
    }
    if (waiters_ == 0) {
      counter_ = 0;
      waiter_for_flush_.notify_all();
    }
  }

 private:
  const size_t participants_;
  size_t counter_ = 0;
  size_t waiters_ = 0;
  twist::ed::stdlike::condition_variable waiter_for_prev_;
  twist::ed::stdlike::condition_variable waiter_for_flush_;
  twist::ed::stdlike::mutex mutex_;
};
