#include <exe/executors/inline.hpp>
#include <exe/executors/task.hpp>

namespace exe::executors {

class InlineExecutor : public IExecutor {
 public:
  // IExecutor
  void Submit(TaskBase* task) override {
    task->Run();
  }
};

IExecutor& Inline() {
  static InlineExecutor instance;
  return instance;
}

}  // namespace exe::executors
