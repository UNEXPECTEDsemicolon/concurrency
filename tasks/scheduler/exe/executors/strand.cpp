#include "strand.hpp"
#include "task.hpp"

namespace exe::executors {

namespace details {

StrandImpl::StrandImpl(IExecutor& underlying)
    : underlying_(underlying) {
}

void StrandImpl::Submit(TaskBase* task) {
  std::lock_guard guard(spinlock_);
  queue_.PushBack(task);
  if (busy_) {
    return;
  }
  busy_ = true;
  this_backup = shared_from_this();
  underlying_.Submit(this);
}

void StrandImpl::Run() noexcept {
  std::unique_lock guard(spinlock_);
  auto limit = queue_.Size();
  for (size_t i = 0; i < limit && !queue_.IsEmpty(); ++i) {
    auto task = queue_.PopFront();
    guard.unlock();
    task->Run();
    guard.lock();
  }
  if (!queue_.IsEmpty()) {
    underlying_.Submit(this);
  } else {
    busy_ = false;
    this_backup.reset();
  }
}

}  // namespace details

Strand::Strand(IExecutor& underlying)
    : impl_(std::make_shared<details::StrandImpl>(underlying)) {
}

void Strand::Submit(TaskBase* task) {
  impl_->Submit(task);
}

}  // namespace exe::executors
