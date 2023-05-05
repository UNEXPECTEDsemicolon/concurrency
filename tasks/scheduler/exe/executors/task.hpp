#pragma once

#include <wheels/intrusive/forward_list.hpp>

namespace exe::executors {

struct ITask {
  virtual ~ITask() = default;

  virtual void Run() noexcept = 0;
};

// Intrusive task
struct TaskBase : ITask,
                  wheels::IntrusiveForwardListNode<TaskBase> {
  //
};

template <typename F,
          typename = std::enable_if_t<std::is_invocable_r_v<void, F>>>
struct FunctionTask : TaskBase {
  explicit FunctionTask(const F& func)
      : func_(func) {
  }
  explicit FunctionTask(F&& func)
      : func_(std::move(func)) {
  }

  void Run() noexcept final {
    func_();
    delete this;
  }

 private:
  F func_;
};

}  // namespace exe::executors
