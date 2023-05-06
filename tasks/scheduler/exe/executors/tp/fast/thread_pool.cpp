#include <cassert>
#include <exe/executors/tp/fast/thread_pool.hpp>
#include "exe/executors/hint.hpp"
#include "exe/executors/tp/fast/worker.hpp"
#include "twist/rt/layer/strand/local/ptr.hpp"

namespace exe::executors::tp::fast {

ThreadPool::ThreadPool(size_t threads)
    : threads_(threads) {
  for (size_t i = 0; i < threads_; ++i) {
    workers_.emplace_back(*this, i);
  }
}

void ThreadPool::Start() {
  for (auto& worker : workers_) {
    worker.Start();
  }
}

ThreadPool::~ThreadPool() {
  assert(stopped_);
}

void ThreadPool::Submit(TaskBase* task) {
  Submit(task, SchedulerHint::UpToYou);
}

void ThreadPool::Submit(TaskBase* task, SchedulerHint hint) {
  auto worker = Worker::Current();
  if (worker != nullptr && &worker->Host() == this &&
      hint != SchedulerHint::Global) {
    worker->Push(task, hint);
  } else {
    global_tasks_.Push(task);
  }
}

void ThreadPool::Stop() {
  stopped_.store(true);
  for (auto& worker : workers_) {
    worker.Join();
  }
}

PoolMetrics ThreadPool::Metrics() const {
  std::abort();
}

ThreadPool* ThreadPool::Current() {
  return Worker::Current() != nullptr ? &Worker::Current()->Host() : nullptr;
}

}  // namespace exe::executors::tp::fast
