#pragma once

#include <exe/executors/executor.hpp>
#include <exe/threads/spinlock.hpp>

#include <queue>

namespace exe::executors {

namespace details {

class StrandImpl : public std::enable_shared_from_this<StrandImpl> {
 public:
  explicit StrandImpl(IExecutor& underlying);

  void Submit(Task);

  void TaskWrapper();

  IExecutor& underlying_;
  std::queue<Task> queue_;
  exe::threads::SpinLock spinlock_;
  bool busy_ = false;
};

}  // namespace details

// Strand / serial executor / asynchronous mutex

class Strand : public IExecutor {
 public:
  explicit Strand(IExecutor& underlying);

  // Non-copyable
  Strand(const Strand&) = delete;
  Strand& operator=(const Strand&) = delete;

  // Non-movable
  Strand(Strand&&) = delete;
  Strand& operator=(Strand&&) = delete;

  // IExecutor
  void Submit(Task cs) override;

 private:
  std::shared_ptr<details::StrandImpl> impl_;
};

}  // namespace exe::executors
