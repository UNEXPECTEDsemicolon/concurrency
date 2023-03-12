#include <exe/fibers/sched/yield.hpp>

namespace exe::fibers {

void Yield() {
  // fibers::Fiber::Self()->Schedule();
  coro::Coroutine::Suspend();
}

}  // namespace exe::fibers
