#include <exe/fibers/sched/go.hpp>
#include <exe/fibers/core/fiber.hpp>
#include "exe/executors/executor.hpp"

namespace exe::fibers {

void Go(executors::IExecutor& scheduler, Routine routine) {
  auto fiber = new Fiber(scheduler, std::move(routine));
  fiber->Schedule();
}

void Go(Routine routine) {
  return Go(*executors::IExecutor::Current(), std::move(routine));
}

}  // namespace exe::fibers
