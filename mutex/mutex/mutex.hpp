#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/sys.hpp>

#include <cstdlib>

namespace stdlike {

class Mutex {
  enum States : uint32_t {
    Locked,
    Unlocked,
  };

 public:
  void Lock() {
    while (locked_.exchange(States::Locked) == States::Locked) {
      twist::ed::Wait(locked_, States::Locked);
    }
  }

  void Unlock() {
    auto wake_key = twist::ed::PrepareWake(locked_);
    locked_.store(States::Unlocked);
    twist::ed::WakeAll(wake_key);
  }

 private:
  twist::ed::stdlike::atomic<uint32_t> locked_{States::Unlocked};
};

}  // namespace stdlike
