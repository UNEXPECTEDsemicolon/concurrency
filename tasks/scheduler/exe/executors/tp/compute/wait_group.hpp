#pragma once

#include <exe/executors/task.hpp>

#include <twist/ed/stdlike/condition_variable.hpp>
#include <twist/ed/stdlike/mutex.hpp>
#include "wheels/core/defer.hpp"

namespace exe::executors::tp::compute::details {

class WaitGroup {
 public:
  void RunTask(TaskBase* task) {
    wheels::Defer([=]() {
      std::lock_guard guard(mutex_);
      running_tasks_--;
      if (running_tasks_ == 0) {
        waiter_.notify_all();
      }
    });
    task->Run();
  }

  void Add() {
    std::lock_guard guard(mutex_);
    running_tasks_++;
  }

  void WaitAll() {
    std::unique_lock guard(mutex_);
    waiter_.wait(guard, [&]() {
      return running_tasks_ == 0;
    });
  }

  size_t running_tasks_ = 0;
  twist::ed::stdlike::condition_variable waiter_;
  twist::ed::stdlike::mutex mutex_;
};

}  // namespace exe::executors::tp::compute::details