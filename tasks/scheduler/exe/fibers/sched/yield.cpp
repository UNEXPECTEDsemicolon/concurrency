#include <exe/fibers/sched/yield.hpp>
#include <exe/fibers/sched/suspend.hpp>
#include "exe/executors/hint.hpp"

namespace exe::fibers {

struct YieldAwaiter : IAwaiter {
  void AwaitSuspend(FiberHandle handle) override {
    handle.Schedule(executors::SchedulerHint::Global);
  }
};

void Yield() {
  auto awaiter = YieldAwaiter();
  Suspend(awaiter);
}

}  // namespace exe::fibers
