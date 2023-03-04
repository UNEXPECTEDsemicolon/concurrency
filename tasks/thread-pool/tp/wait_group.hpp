#pragma once

#include <tp/task.hpp>

#include <twist/ed/stdlike/condition_variable.hpp>
#include <twist/ed/stdlike/mutex.hpp>

namespace tp {

class WaitGroup {
 public:
  void RunTask(Task task) {
    Runner(this, std::move(task));
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

 private:
  class Runner {
   public:
    Runner(WaitGroup* wait_group, Task task)
        : wait_group_(wait_group) {
      task();
    }

    ~Runner() {
      std::lock_guard guard(wait_group_->mutex_);
      wait_group_->running_tasks_--;
      if (wait_group_->running_tasks_ == 0) {
        wait_group_->waiter_.notify_all();
      }
    }

   private:
    WaitGroup* wait_group_;
  };

  size_t running_tasks_ = 0;
  twist::ed::stdlike::condition_variable waiter_;
  twist::ed::stdlike::mutex mutex_;
};

}  // namespace tp