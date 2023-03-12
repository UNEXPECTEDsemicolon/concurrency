#include <exe/fibers/core/fiber.hpp>

#include <twist/ed/local/ptr.hpp>
#include "exe/coro/core.hpp"

namespace exe::fibers {

Fiber::Fiber(Scheduler& scheduler, Routine routine)
    : coroutine_(std::move(routine)),
      scheduler_(&scheduler) {
}

void Fiber::Schedule() {
  scheduler_->Submit([&]() {
    Run();
  });
}

void Fiber::Run() {
  coro::Coroutine::GetCallstack() = co_callstack_;
  coroutine_.Resume();
  co_callstack_ = coro::Coroutine::GetCallstack();
  if (!coroutine_.IsCompleted()) {
    Schedule();
  } else {
    delete this;
  }
}

}  // namespace exe::fibers
