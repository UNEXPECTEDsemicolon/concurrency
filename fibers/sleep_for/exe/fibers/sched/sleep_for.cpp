#include <exe/fibers/sched/sleep_for.hpp>

#include <asio/steady_timer.hpp>
#include <exe/fibers/core/fiber.hpp>
#include <exe/fibers/sched/yield.hpp>
#include "asio/defer.hpp"
#include "asio/post.hpp"

namespace exe::fibers {

void SleepFor(Millis delay) {
  auto cur_fiber = Fiber::Self();
  auto timer = new asio::steady_timer(cur_fiber->GetScheduler());
  timer->expires_after(delay);

  timer->async_wait([=](std::error_code) {
    while (cur_fiber->IsRunning()) {
    }
    cur_fiber->Schedule();
    delete timer;
  });
  coro::Coroutine::Suspend();
}

}  // namespace exe::fibers
