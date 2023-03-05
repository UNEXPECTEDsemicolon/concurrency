#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/sys.hpp>

#include <cstdlib>

namespace stdlike {

class Mutex {
 public:
  void Lock() {
    uint32_t temp = States::Unlocked;
    if (state_.compare_exchange_strong(temp, States::Locked)) {
      return;
    }
    while (state_.exchange(States::Queue) != States::Unlocked) {
      twist::ed::Wait(state_, States::Queue);
    }
  }

  void Unlock() {
    if (state_.fetch_sub(1) == States::Locked) {
      return;
    }
    auto wake_key = twist::ed::PrepareWake(state_);
    state_.store(States::Unlocked);
    twist::ed::WakeOne(wake_key);
  }

 private:
  enum States : uint32_t {
    Unlocked = 0,
    Locked = 1,
    Queue = 2,
  };

  twist::ed::stdlike::atomic<uint32_t> state_{States::Unlocked};
};

}  // namespace stdlike
