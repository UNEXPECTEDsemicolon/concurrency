#pragma once

#include <exe/executors/executor.hpp>
#include <exe/threads/spinlock.hpp>
#include <exe/executors/task.hpp>

#include <memory>
#include <wheels/intrusive/forward_list.hpp>

namespace exe::executors {

namespace details {

class StrandImpl : public std::enable_shared_from_this<StrandImpl>,
                   TaskBase {
 public:
  explicit StrandImpl(IExecutor& underlying);

  void Submit(TaskBase*);

  void Run() noexcept;

  IExecutor& underlying_;
  wheels::IntrusiveForwardList<TaskBase> queue_;
  exe::threads::SpinLock spinlock_;
  std::shared_ptr<StrandImpl> this_backup;
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
  void Submit(TaskBase* cs) override;

 private:
  std::shared_ptr<details::StrandImpl> impl_;
};

}  // namespace exe::executors
