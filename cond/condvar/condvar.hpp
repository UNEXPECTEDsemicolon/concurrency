#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/sys.hpp>

#include <cstdint>

namespace stdlike {

class CondVar {
 public:
  // Mutex - BasicLockable
  // https://en.cppreference.com/w/cpp/named_req/BasicLockable
  template <class Mutex>
  void Wait(Mutex& mutex) {
    auto old_ticket = ticket_.load();
    mutex.unlock();
    twist::ed::Wait(ticket_, old_ticket);
    mutex.lock();
  }

  void NotifyOne() {
    auto wake_key = twist::ed::PrepareWake(ticket_);
    ticket_++;
    twist::ed::WakeOne(wake_key);
  }

  void NotifyAll() {
    auto wake_key = twist::ed::PrepareWake(ticket_);
    ticket_++;
    twist::ed::WakeAll(wake_key);
  }

 private:
  twist::ed::stdlike::atomic<uint32_t> ticket_{0};
};

}  // namespace stdlike
