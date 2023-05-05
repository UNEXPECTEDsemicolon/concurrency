#pragma once

// std::lock_guard and std::unique_lock
#include <mutex>
#include <utility>

#include <exe/fibers/sched/suspend.hpp>
#include <wheels/intrusive/list.hpp>
#include <exe/threads/spinlock.hpp>

namespace exe::fibers {

namespace details {

struct MutexAwaiter;

}  // namespace details

class Mutex {
 public:
  void Lock();

  void Unlock();

  // BasicLockable

  void lock() {  // NOLINT
    Lock();
  }

  void unlock() {  // NOLINT
    Unlock();
  }

 private:
  friend struct details::MutexAwaiter;

  bool locked_ = false;
  threads::SpinLock spinlock_;
  wheels::IntrusiveList<details::MutexAwaiter> queue_;
};

namespace details {

struct MutexAwaiter : IAwaiter,
                      wheels::IntrusiveListNode<MutexAwaiter> {
  explicit MutexAwaiter(Mutex* event)
      : mutex_(event) {
  }

  void AwaitSuspend(FiberHandle handle) override {
    handle_ = handle;
    std::lock_guard guard(mutex_->spinlock_);
    if (mutex_->locked_) {
      mutex_->queue_.PushBack(this);
    } else {
      Fire();
    }
  }

  void Fire() {
    handle_.Schedule();
  }

  FiberHandle handle_;
  Mutex* mutex_;
};

}  // namespace details

void Mutex::Lock() {
  std::unique_lock guard(spinlock_);
  while (std::exchange(locked_, true)) {
    details::MutexAwaiter awaiter(this);
    guard.unlock();
    fibers::Suspend(awaiter);
    guard.lock();
  }
}

void Mutex::Unlock() {
  std::lock_guard guard(spinlock_);
  locked_ = false;
  if (queue_.NonEmpty()) {
    queue_.PopFront()->Fire();
  }
}

}  // namespace exe::fibers