#pragma once

#include <tp/queue.hpp>
#include <tp/task.hpp>
#include <tp/wait_group.hpp>

#include <twist/ed/stdlike/thread.hpp>
#include <twist/ed/stdlike/atomic.hpp>
#include <vector>

namespace tp {

// Fixed-size pool of worker threads

class ThreadPool {
 public:
  explicit ThreadPool(size_t threads);
  ~ThreadPool();

  // Non-copyable
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  // Non-movable
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  // Launches worker threads
  void Start();

  // Schedules task for execution in one of the worker threads
  void Submit(Task);

  // Locates current thread pool from worker thread
  static ThreadPool* Current();

  // Waits until outstanding work count reaches zero
  void WaitIdle();

  // Stops the worker threads as soon as possible
  void Stop();

 private:
  void Worker();

  const size_t thread_cnt_;
  std::vector<twist::ed::stdlike::thread> workers_;
  UnboundedBlockingQueue<Task> task_queue_;
  twist::ed::stdlike::atomic<bool> stopped_ = false;
  WaitGroup wait_group_;
};

}  // namespace tp
