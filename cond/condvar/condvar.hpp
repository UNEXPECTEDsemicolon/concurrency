#pragma once

#include <sys/types.h>
#include <iterator>
#include <memory>
#include <queue>
#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/sys.hpp>
#include <list>
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
    auto ptr = waiters_.emplace_back(
        std::make_shared<twist::ed::stdlike::atomic<uint32_t>>(1));
    my_mutex_.unlock();
    mutex.unlock();
    twist::ed::Wait(*ptr, 1);
    mutex.lock();
  }

  void NotifyOne() {
    my_mutex_.lock();
    if (!waiters_.empty()) {
      auto& ind = *waiters_.front();  // TODO:
      waiters_.pop_front();
      my_mutex_.unlock();
      auto wake_key = twist::ed::PrepareWake(ind);
      ind.store(0);
      twist::ed::WakeOne(wake_key);
    } else {
      my_mutex_.unlock();
    }
  }

  void NotifyAll() {
    while (!waiters_.empty()) {
      NotifyOne();
    }
  }

 private:
  std::list<std::shared_ptr<twist::ed::stdlike::atomic<uint32_t>>> waiters_;
  twist::ed::stdlike::atomic<uint32_t> locked_{0};
  Mutex my_mutex_;
};

}  // namespace stdlike
