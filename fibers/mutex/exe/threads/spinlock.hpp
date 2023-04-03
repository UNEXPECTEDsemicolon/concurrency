#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/spin/wait.hpp>

namespace exe::threads {

// Test-and-TAS spinlock

class SpinLock {
 public:
  void Lock() {
    twist::ed::SpinWait spin_wait;
    while (!TryLock()) {
      while (locked_.load()) {
        spin_wait();
      }
    }
  }

  bool TryLock() {
    return !locked_.exchange(true);
  }

  void Unlock() {
    locked_.store(false);
  }

  // Lockable

  void lock() {  // NOLINT
    Lock();
  }

  bool try_lock() {  // NOLINT
    return TryLock();
  }

  void unlock() {  // NOLINT
    Unlock();
  }

 private:
  twist::ed::stdlike::atomic<bool> locked_{false};
};

}  // namespace exe::threads
