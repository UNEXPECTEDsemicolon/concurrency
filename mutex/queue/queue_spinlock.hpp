#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/spin.hpp>

/*
 * Scalable Queue SpinLock
 *
 * Usage:
 *
 * QueueSpinLock spinlock;
 *
 * {
 *   QueueSpinLock::Guard guard(spinlock);  // <-- Acquire
 *   // <-- Critical section
 * }  // <-- Release
 *
 */

class QueueSpinLock {
 public:
  class Guard {
    friend class QueueSpinLock;

   public:
    explicit Guard(QueueSpinLock& host)
        : host_(host) {
      host_.Acquire(this);
    }

    ~Guard() {
      host_.Release(this);
    }

   private:
    QueueSpinLock& host_;
    twist::ed::stdlike::atomic<Guard*> next_ = nullptr;
    twist::ed::stdlike::atomic<bool> is_owner_ = false;
  };

 private:
  void Acquire(Guard* waiter) {
    auto old_tail = tail_.exchange(waiter);
    if (old_tail == nullptr) {
      waiter->is_owner_ = true;
      return;
    }
    old_tail->next_ = waiter;
    twist::ed::SpinWait spin_wait;
    while (!waiter->is_owner_) {
      spin_wait.Spin();
    }
  }

  void Release(Guard* owner) {
    auto owner_copy = owner;
    if (!tail_.compare_exchange_strong(owner_copy, nullptr)) {
      twist::ed::SpinWait spin_wait;
      while (owner->next_ == nullptr) {
        spin_wait.Spin();
      }
      owner->next_.load()->is_owner_ = true;
    }
  }

 private:
  twist::ed::stdlike::atomic<Guard*> tail_ = nullptr;
};
