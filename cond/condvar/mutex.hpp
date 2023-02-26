#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/./stdlike/mutex.hpp>
#include <twist/ed/wait/sys.hpp>

#include <cstdint>

namespace stdlike {

class Mutex : public twist::ed::stdlike::mutex {
 public:
  void Lock() {
    return lock();
  }

  void Unlock() {
    return unlock();
  }
};

}  // namespace stdlike
