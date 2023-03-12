#include <exe/fibers/sched/yield.hpp>

namespace exe::fibers {

void Yield() {
  coro::Coroutine::Suspend();
}

}  // namespace exe::fibers
