#pragma once

#include <exe/executors/tp/compute/thread_pool.hpp>

// namespace exe::executors::tp::fast {

// using ThreadPool = compute::ThreadPool;

// }

#include <exe/executors/executor.hpp>

#include <exe/executors/tp/fast/queues/global_queue.hpp>
#include <exe/executors/tp/fast/worker.hpp>
#include <exe/executors/tp/fast/coordinator.hpp>
#include <exe/executors/tp/fast/metrics.hpp>

// random_device
#include <twist/ed/stdlike/random.hpp>

#include <deque>
#include "twist/ed/stdlike/atomic.hpp"

namespace exe::executors::tp::fast {

// Scalable work-stealing scheduler for short-lived tasks

class ThreadPool : public IExecutor {
  friend class Worker;

 public:
  explicit ThreadPool(size_t threads);
  ~ThreadPool();

  // Non-copyable
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  void Start();

  // IExecutor
  void Submit(TaskBase*) override;

  void Submit(TaskBase*, SchedulerHint) override;

  void Stop();

  // After Stop
  PoolMetrics Metrics() const;

  static ThreadPool* Current();

 private:
  const size_t threads_;
  std::deque<Worker> workers_;
  Coordinator coordinator_;
  GlobalQueue global_tasks_;
  twist::ed::stdlike::random_device random_;
  twist::ed::stdlike::atomic<bool> stopped_ = false;
  twist::ed::stdlike::atomic<size_t> stealers_ = 0;
};

}  // namespace exe::executors::tp::fast
