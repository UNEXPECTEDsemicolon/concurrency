#pragma once

#include <exe/executors/thread_pool.hpp>
#include "exe/executors/executor.hpp"
#include "exe/executors/task.hpp"

namespace exe::fibers {

template <typename Executor>
struct Scheduler {
  Scheduler(Executor&);

  inline void Submit(executors::TaskBase* task) {
    exe_.Submit(task);
  }

  static Executor* Current();

 private:
  executors::IExecutor& exe_;
};

template <>
struct Scheduler<executors::ThreadPool> {
  Scheduler(executors::ThreadPool&);

  inline void Submit(executors::TaskBase* task) {
    exe_.Submit(task);
  }

  static executors::ThreadPool* Current() {
    return executors::ThreadPool::Current();
  }

 private:
  executors::IExecutor& exe_;
};

// using Scheduler = executors::ThreadPool;

}  // namespace exe::fibers
