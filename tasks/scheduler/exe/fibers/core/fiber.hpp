#pragma once

#include <exe/fibers/core/routine.hpp>
#include <exe/executors/executor.hpp>
#include <exe/fibers/core/awaiter.hpp>

#include <exe/coro/core.hpp>
#include "exe/fibers/core/scheduler.hpp"

namespace exe::fibers {

// Fiber = stackful coroutine + scheduler (executor)

class Fiber : exe::executors::TaskBase {
 public:
  Fiber(executors::IExecutor&, Routine);

  void Suspend(IAwaiter& awaiter);

  void Schedule();
  void Switch();

  // Task
  void Run() noexcept override;

  static Fiber* Self();

 private:
  coro::Coroutine coroutine_;
  coro::Coroutine::CallStack* co_callstack_ = nullptr;
  executors::IExecutor& scheduler_;
  IAwaiter* awaiter_;
};

}  // namespace exe::fibers
