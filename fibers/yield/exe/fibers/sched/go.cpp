#include <exe/fibers/sched/go.hpp>

namespace exe::fibers {

void Go(Scheduler& scheduler, Routine routine) {
  auto fiber = new Fiber(scheduler, std::move(routine));
  fiber->Schedule();
}

void Go(Routine routine) {
  return Go(*tp::ThreadPool::Current(), std::move(routine));
}

}  // namespace exe::fibers
