#include <exe/executors/manual.hpp>

namespace exe::executors {

void ManualExecutor::Submit(Task task) {
  queue_.push(std::move(task));
}

// Run tasks

size_t ManualExecutor::RunAtMost(size_t limit) {
  for (size_t i = 0; i < limit; ++i) {
    if (queue_.empty()) {
      return i;
    }
    PopAndRun();
  }
  return limit;
}

size_t ManualExecutor::Drain() {
  return RunAtMost(-1);
  // size_t result = 0;
  // for (; !queue_.empty(); ++result) {
  //   PopAndRun();
  // }
  // return result
}

void ManualExecutor::PopAndRun() {
  auto task = std::move(queue_.front());
  queue_.pop();
  task();
}

}  // namespace exe::executors
