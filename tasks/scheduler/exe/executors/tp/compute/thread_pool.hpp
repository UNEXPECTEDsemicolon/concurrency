#pragma once

#include <exe/executors/executor.hpp>
#include <exe/executors/tp/compute/queue.hpp>
#include <exe/executors/tp/compute/wait_group.hpp>

#include <twist/ed/stdlike/thread.hpp>
#include <twist/ed/stdlike/atomic.hpp>
#include <vector>
#include "exe/executors/task.hpp"

namespace exe::executors::tp::compute {

// Thread pool for independent CPU-bound tasks
// Fixed pool of worker threads + shared unbounded blocking queue

class ThreadPool : public IExecutor {
 public:
  explicit ThreadPool(size_t threads);
  ~ThreadPool();

  // Non-copyable
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  // Non-movable
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  void Start();

  // IExecutor
  void Submit(TaskBase*) override;

  static ThreadPool* Current();

  void Stop();

 private:
  void Worker();

  const size_t thread_cnt_;
  std::vector<twist::ed::stdlike::thread> workers_;
  details::UnboundedBlockingQueue<TaskBase> task_queue_;
  twist::ed::stdlike::atomic<bool> stopped_ = false;
  details::WaitGroup wait_group_;
};

}  // namespace exe::executors::tp::compute
