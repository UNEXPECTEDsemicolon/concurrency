#pragma once

#include <exe/fibers/core/routine.hpp>
#include <exe/fibers/core/scheduler.hpp>
#include <exe/fibers/core/awaiter.hpp>

#include <exe/coro/core.hpp>

namespace exe::fibers {

// Fiber = stackful coroutine + scheduler (executor)

class Fiber {
 public:
  Fiber(Scheduler&, Routine);

  void Suspend(IAwaiter& awaiter);

  void Schedule();
  void Switch();

  // Task
  void Run();

  static Fiber* Self();

 private:
  coro::Coroutine coroutine_;
  coro::Coroutine::CallStack* co_callstack_ = nullptr;
  Scheduler* scheduler_;
  IAwaiter* awaiter_;
};

}  // namespace exe::fibers
