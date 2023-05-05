#include <exe/fibers/core/fiber.hpp>

#include <twist/ed/local/ptr.hpp>
#include "exe/fibers/core/scheduler.hpp"

namespace exe::fibers {

static twist::ed::ThreadLocalPtr<Fiber> running_fiber;

Fiber::Fiber(executors::IExecutor& scheduler, Routine routine)
    : coroutine_(std::move(routine)),
      scheduler_(scheduler) {
}

void Fiber::Suspend(IAwaiter& awaiter) {
  awaiter_ = &awaiter;
  coro::Coroutine::Suspend();
}

void Fiber::Schedule() {
  scheduler_.Submit(this);
}

void Fiber::Switch() {
  coro::Coroutine::GetCallstack() = co_callstack_;
  coroutine_.Resume();
  co_callstack_ = coro::Coroutine::GetCallstack();
}

void Fiber::Run() noexcept {
  running_fiber = this;
  Switch();
  if (!coroutine_.IsCompleted()) {
    awaiter_->AwaitSuspend(FiberHandle(this));
  } else {
    delete this;
  }
}

Fiber* Fiber::Self() {
  return running_fiber;
}

}  // namespace exe::fibers
