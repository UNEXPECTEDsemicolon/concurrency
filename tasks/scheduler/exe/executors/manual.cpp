#include "manual.hpp"

namespace exe::executors {

void ManualExecutor::Submit(TaskBase* task) {
  queue_.PushBack(task);
}

// Run tasks

size_t ManualExecutor::RunAtMost(size_t limit) {
  for (size_t i = 0; i < limit; ++i) {
    if (queue_.IsEmpty()) {
      return i;
    }
    PopAndRun();
  }
  return limit;
}

size_t ManualExecutor::Drain() {
  SetCurrent(this);
  return RunAtMost(-1);
}

void ManualExecutor::PopAndRun() {
  std::move(queue_.PopFront())->Run();
}

}  // namespace exe::executors
