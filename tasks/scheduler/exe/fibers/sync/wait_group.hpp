#pragma once

#include <cstdlib>
#include <twist/ed/stdlike/atomic.hpp>
#include "event.hpp"

namespace exe::fibers {

// https://gobyexample.com/waitgroups

class WaitGroup {
 public:
  void Add(size_t count) {
    cnt_.fetch_add(count);
  }

  void Done() {
    if (cnt_.fetch_sub(1) == 1) {
      event_.Fire();
    }
  }

  void Wait() {
    event_.Wait();
  }

 private:
  Event event_;
  twist::ed::stdlike::atomic<size_t> cnt_ = 0;
};

}  // namespace exe::fibers
