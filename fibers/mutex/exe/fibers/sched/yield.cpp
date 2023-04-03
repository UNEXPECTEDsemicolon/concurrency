#include <exe/fibers/sched/yield.hpp>

namespace exe::fibers {

struct YieldAwaiter : IAwaiter {
  void AwaitSuspend(FiberHandle handle) override {
    handle.Schedule();
  }
};

void Yield() {
  auto awaiter = YieldAwaiter();
  Suspend(awaiter);
}

}  // namespace exe::fibers
