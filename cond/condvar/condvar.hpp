#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/sys.hpp>
#include "mutex.hpp"

#include <cstdint>

namespace stdlike {

class CondVar {
 public:
  // Mutex - BasicLockable
  // https://en.cppreference.com/w/cpp/named_req/BasicLockable
  template <class Mutex>
  void Wait(Mutex& mutex) {
    twist::ed::stdlike::atomic<uint32_t> locked{1};
    {
      std::lock_guard queue_guard(queue_mutex_);
      waiters_.emplace(&locked);
    }
    mutex.unlock();
    twist::ed::Wait(locked, 1);
    mutex.lock();
  }

  void NotifyOne() {
    std::lock_guard guard(queue_mutex_);
    if (!waiters_.empty()) {
      NotifyOneImpl();
    }
  }

  void NotifyAll() {
    std::lock_guard guard(queue_mutex_);
    while (!waiters_.empty()) {
      NotifyOneImpl();
    }
  }

 private:
  void NotifyOneImpl() {
    auto& ind = *waiters_.front();
    waiters_.pop();
    auto wake_key = twist::ed::PrepareWake(ind);
    ind.store(0);
    twist::ed::WakeOne(wake_key);
  }

  std::queue<twist::ed::stdlike::atomic<uint32_t>*> waiters_;
  Mutex queue_mutex_;
};

}  // namespace stdlike
