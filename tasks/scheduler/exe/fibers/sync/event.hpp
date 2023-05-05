#pragma once

#include <exe/fibers/sched/suspend.hpp>
#include <mutex>
#include <wheels/intrusive/list.hpp>
#include <exe/threads/spinlock.hpp>
// #include "exe/fibers/core/awaiter.hpp"

namespace exe::fibers {

namespace details {

struct EventAwaiter;

}  // namespace details

// One-shot

class Event {
 public:
  void Wait();

  void Fire();

 private:
  friend struct details::EventAwaiter;

  bool fired_ = false;
  threads::SpinLock spinlock_;
  wheels::IntrusiveList<details::EventAwaiter> queue_;
};

namespace details {

struct EventAwaiter : IAwaiter,
                      wheels::IntrusiveListNode<EventAwaiter> {
  explicit EventAwaiter(Event* event)
      : event_(event) {
  }

  void AwaitSuspend(FiberHandle handle) override {
    handle_ = handle;
    std::lock_guard guard(event_->spinlock_);
    if (!event_->fired_) {
      event_->queue_.PushBack(this);
    } else {
      Fire();
    }
  }

  void Fire() {
    handle_.Schedule();
  }

  FiberHandle handle_;
  Event* event_;
};

}  // namespace details

void Event::Wait() {
  details::EventAwaiter awaiter(this);
  fibers::Suspend(awaiter);
  std::lock_guard guard(spinlock_);
}

void Event::Fire() {
  std::lock_guard guard(spinlock_);
  fired_ = true;
  while (queue_.NonEmpty()) {
    queue_.PopFront()->Fire();
  }
}

}  // namespace exe::fibers
