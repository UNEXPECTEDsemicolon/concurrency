#include <exe/fibers/core/fiber.hpp>
#include <asio/post.hpp>
#include <twist/ed/local/ptr.hpp>
#include <asio/defer.hpp>
// #include <exe/fibers/core/scheduler.hpp>

namespace exe::fibers {

static twist::ed::ThreadLocalPtr<Fiber> running_fiber;

Fiber::Fiber(Scheduler& scheduler, Routine routine)
    : coroutine_(std::move(routine)),
      scheduler_(&scheduler) {
}

void Fiber::Schedule() {
  asio::post(*scheduler_, [&]() {
    Run();
  });
}

void Fiber::Run() {
  running_ = true;
  if (!coroutine_.IsCompleted()) {
    running_fiber = this;
    coro::Coroutine::GetCallstack() = co_callstack_;
    coroutine_.Resume();
    co_callstack_ = coro::Coroutine::GetCallstack();
  }
  if (coroutine_.IsCompleted()) {
    running_ = false;
    delete this;
  } else {
    running_ = false;
  }
}

Scheduler& Fiber::GetScheduler() {
  return *scheduler_;
}

Fiber* Fiber::Self() {
  return running_fiber;
}

}  // namespace exe::fibers
