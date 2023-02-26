#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/sys.hpp>

#include <cstdint>

namespace stdlike {

class Mutex {
  enum States : uint32_t {
    Locked,
    Unlocked,
  };

 public:
  void Lock() {
    waiters_++;
    while (locked_.exchange(States::Locked) == States::Locked) {
      twist::ed::Wait(locked_, States::Locked);
    }
    waiters_--;
  }

  void Unlock() {
    auto wake_key = twist::ed::PrepareWake(locked_);
    locked_.store(States::Unlocked);
    if (waiters_ != 0) {
      twist::ed::WakeAll(wake_key);
    }
  }

  // BasicLockable
  // https://en.cppreference.com/w/cpp/named_req/BasicLockable

  void lock() {  // NOLINT
    Lock();
  }

  void unlock() {  // NOLINT
    Unlock();
  }

 private:
  twist::ed::stdlike::atomic<uint32_t> locked_{States::Unlocked};
  twist::ed::stdlike::atomic<uint32_t> waiters_{0};
};

}  // namespace stdlike
