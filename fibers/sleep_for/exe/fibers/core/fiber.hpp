#pragma once

#include <exe/fibers/core/routine.hpp>
#include <exe/fibers/core/scheduler.hpp>

#include <exe/coro/core.hpp>
#include "twist/ed/stdlike/atomic.hpp"

namespace exe::fibers {

// Fiber = stackful coroutine + scheduler (thread pool)

class Fiber {
 public:
  Fiber(Scheduler&, Routine);

  void Schedule();

  Scheduler& GetScheduler();

  inline auto& IsRunning() const {
    return running_;
  };

  static Fiber* Self();

 private:
  // Task
  void Run();
  coro::Coroutine coroutine_;
  coro::Coroutine::CallStack* co_callstack_ = nullptr;
  Scheduler* scheduler_;
  twist::ed::stdlike::atomic<bool> running_;
};

}  // namespace exe::fibers
