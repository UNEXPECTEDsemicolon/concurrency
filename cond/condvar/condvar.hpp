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
  CondVar() {
    // waiters_.emplace_back(1);
  }

  // Mutex - BasicLockable
  // https://en.cppreference.com/w/cpp/named_req/BasicLockable
  template <class Mutex>
  void Wait(Mutex& mutex) {
    // mutex.unlock();
    // while (cnt_ == 0) {
    //   waiters_++;
    //   twist::ed::Wait(cnt_, 0);
    //   waiters_--;
    //   auto val = cnt_.load();
    //   while (val > 0 && !cnt_.compare_exchange_strong(val, val-1)) {
    //     val = cnt_.load();
    //   }
    //   if (val != 0) {
    //     break;
    //   }
    // }
    // mutex.lock();

    // mutex.unlock();
    // twist::ed::Wait(locked_, 1);
    // mutex.lock();
    // locked_.store(1);

    // auto origin = locked_++;
    // while (!safe_.exchange(0)) {
    //   twist::ed::Wait(safe_, 0);
    // }
    // uint32_t temp = -1;
    // while (!locked_.compare_exchange_strong(temp, 1)) {
    // twist::ed::Wait(locked_, temp);
    // temp = -1;
    // }
    // mutex.unlock();
    // while (!wake_all_ && unsafe_.exchange(1)) {
    //   twist::ed::Wait(unsafe_, 1);
    // }
    // if (wake_all_) {
    //   mutex.lock();
    //   return;
    // }
    // my_mutex_.lock();
    // locked_.store(1);
    // my_mutex_.unlock();

    // twist::ed::Wait(locked_, 1);

    // mutex.lock();

    // auto wake_key = twist::ed::PrepareWake(unsafe_);
    // unsafe_.store(0);
    // twist::ed::WakeAll(wake_key);

    // locked_.store(1);
    //===========================
    my_mutex_.lock();
    // if (cnt_ == 0) {
    auto ptr = waiters_.emplace_back(
        std::make_shared<twist::ed::stdlike::atomic<uint32_t>>(1));
    my_mutex_.unlock();
    mutex.unlock();
    twist::ed::Wait(*ptr, 1);
    // } else {
    //   cnt_--;

    //   my_mutex_.unlock();
    // }
    mutex.lock();
    // =============================================
    // my_mutex_.lock();
    // waiters_.erase(ind);
    // my_mutex_.unlock();
  }

  void NotifyOne() {
    // my_mutex_.lock();
    // auto wake_key = twist::ed::PrepareWake(locked_);
    // locked_.store(0);
    // twist::ed::WakeOne(wake_key);
    // my_mutex_.unlock();

    //==============
    my_mutex_.lock();
    if (!waiters_.empty()) {
      auto& ind = *waiters_.front();  // TODO:
      waiters_.pop_front();
      my_mutex_.unlock();
      auto wake_key = twist::ed::PrepareWake(ind);
      ind.store(0);
      twist::ed::WakeOne(wake_key);
    } else {
      cnt_++;
      my_mutex_.unlock();
    }
    //==============

    // auto wake_key = twist::ed::PrepareWake(locked_);
    // locked_.store(0);
    // twist::ed::WakeOne(wake_key);
  }

  void NotifyAll() {
    //========================
    // my_mutex_.lock();
    while (!waiters_.empty()) {
      NotifyOne();
    }
    // for (auto& ind: waiters_) {
    //   waiters_.pop_front();
    //   auto wake_key = twist::ed::PrepareWake(*ind);
    //   ind->store(0);
    //   twist::ed::WakeOne(wake_key);
    // }
    // my_mutex_.unlock();
    //========================
    // auto wake_key = twist::ed::PrepareWake(locked_);
    // locked_.store(0);
    // twist::ed::WakeAll(wake_key);
  }

 private:
  std::list<std::shared_ptr<twist::ed::stdlike::atomic<uint32_t>>> waiters_;
  twist::ed::stdlike::atomic<uint32_t> cnt_;
  // twist::ed::stdlike::atomic<uint32_t> waiters_;
  // twist::ed::stdlike::atomic<uint32_t> current_;
  // twist::ed::stdlike::atomic<uint32_t> ticket_;
  twist::ed::stdlike::atomic<uint32_t> locked_{0};
  twist::ed::stdlike::atomic<uint32_t> after_wake_{0};
  twist::ed::stdlike::atomic<uint32_t> wake_all_{0};
  Mutex my_mutex_;
};

}  // namespace stdlike
