#include <cassert>
#include <exe/executors/tp/compute/thread_pool.hpp>

#include <twist/ed/local/ptr.hpp>

#include <wheels/core/panic.hpp>

namespace exe::executors::tp::compute {

static twist::ed::ThreadLocalPtr<ThreadPool> pool = nullptr;

ThreadPool::ThreadPool(size_t threads)
    : thread_cnt_(threads) {
}

void ThreadPool::Start() {
  for (size_t i = 0; i < thread_cnt_; ++i) {
    workers_.emplace_back([this]() {
      Worker();
    });
  }
}

void ThreadPool::Worker() {
  pool = this;
  SetCurrent(this);
  while (!stopped_) {
    auto task = task_queue_.Take();
    if (task.has_value()) {
      wait_group_.RunTask(*task);
    }
  }
}

ThreadPool::~ThreadPool() {
  assert(stopped_);
}

void ThreadPool::Submit(TaskBase* task) {
  wait_group_.Add();
  task_queue_.Put(task);
}

ThreadPool* ThreadPool::Current() {
  return pool;
}

void ThreadPool::Stop() {
  task_queue_.Close();
  stopped_ = true;
  for (auto& worker : workers_) {
    worker.join();
  }
}

}  // namespace exe::executors::tp::compute
