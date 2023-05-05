#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/futex.hpp>

#include <cstdlib>

namespace exe::threads {

class WaitGroup {
 public:
  // += count
  void Add(size_t count) {
    if (waiter_.load() == 0) {
      waiter_.store(1);
    }
    cnt_.fetch_add(count);
  }

  // -= 1
  void Done() {
    if (cnt_.fetch_sub(1) == 1) {
      auto key = twist::ed::futex::PrepareWake(waiter_);
      waiter_.store(0);
      twist::ed::futex::WakeAll(key);
    }
  }

  // == 0
  void Wait() {
    twist::ed::futex::Wait(waiter_, 1);
  }

 private:
  twist::ed::stdlike::atomic<size_t> cnt_ = 0;
  twist::ed::stdlike::atomic<uint32_t> waiter_ = 0;
};

}  // namespace exe::threads
