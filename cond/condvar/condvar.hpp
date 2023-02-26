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
    my_mutex_.lock();
    auto ptr = waiters_.emplace(
        std::make_shared<twist::ed::stdlike::atomic<uint32_t>>(1));
    my_mutex_.unlock();
    mutex.unlock();
    twist::ed::Wait(*ptr, 1);
    mutex.lock();
  }

  void NotifyOne() {
    std::lock_guard guard(my_mutex_);
    if (!waiters_.empty()) {
      NotifyOneImpl();
    }
  }

  void NotifyAll() {
    std::lock_guard guard(my_mutex_);
    while (!waiters_.empty()) {
      NotifyOneImpl();
    }
  }

 private:
  void NotifyOneImpl() {
    auto& ind = *waiters_.front();  // TODO:
    waiters_.pop();
    auto wake_key = twist::ed::PrepareWake(ind);
    ind.store(0);
    twist::ed::WakeOne(wake_key);
  }

  std::queue<std::shared_ptr<twist::ed::stdlike::atomic<uint32_t>>> waiters_;
  twist::ed::stdlike::atomic<uint32_t> locked_{0};
  Mutex my_mutex_;
};

}  // namespace stdlike
