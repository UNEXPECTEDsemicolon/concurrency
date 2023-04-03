#pragma once

#include <exe/fibers/sched/suspend.hpp>
#include <mutex>
#include <wheels/intrusive/list.hpp>
#include <exe/threads/spinlock.hpp>

namespace exe::fibers {

namespace details {

struct FutexAwaiter;

}  // namespace details

class Futex {
 public:
  void Wait();

  void WakeOne();

  void WakeAll();

 private:
  friend struct details::FutexAwaiter;

  bool fired_ = false;
  threads::SpinLock spinlock_;
  wheels::IntrusiveList<details::FutexAwaiter> queue_;
};

namespace details {

struct FutexAwaiter : IAwaiter,
                      wheels::IntrusiveListNode<FutexAwaiter> {
  explicit FutexAwaiter(Futex* futex)
      : futex_(futex) {
  }

  void AwaitSuspend(FiberHandle handle) override {
    handle_ = handle;
    std::lock_guard guard(futex_->spinlock_);
    if (!futex_->fired_) {
      futex_->queue_.PushBack(this);
    } else {
      Fire();
    }
  }

  void Fire() {
    handle_.Schedule();
  }

  FiberHandle handle_;
  Futex* futex_;
};

}  // namespace details

void Futex::Wait() {
  details::FutexAwaiter awaiter(this);
  fibers::Suspend(awaiter);
  std::lock_guard guard(spinlock_);
}

void Futex::WakeOne() {
  std::lock_guard guard(spinlock_);
  if (queue_.NonEmpty()) {
    queue_.PopFront()->Fire();
  }
}

void Futex::WakeAll() {
  std::lock_guard guard(spinlock_);
  fired_ = true;
  while (queue_.NonEmpty()) {
    queue_.PopFront()->Fire();
  }
}

}  // namespace exe::fibers
