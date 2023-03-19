#include <exe/executors/strand.hpp>

namespace exe::executors {

namespace details {

StrandImpl::StrandImpl(IExecutor& underlying)
    : underlying_(underlying) {
}

void StrandImpl::Submit(Task task) {
  std::lock_guard guard(spinlock_);
  queue_.push(std::move(task));
  if (busy_) {
    return;
  }
  busy_ = true;
  underlying_.Submit([this_backup = shared_from_this()] {
    this_backup->TaskWrapper();
  });
}

void StrandImpl::TaskWrapper() {
  std::unique_lock guard(spinlock_);
  auto limit = queue_.size();
  for (size_t i = 0; i < limit && !queue_.empty(); ++i) {
    auto task = std::move(queue_.front());
    queue_.pop();
    guard.unlock();
    task();
    guard.lock();
  }
  if (!queue_.empty()) {
    underlying_.Submit([this_backup = shared_from_this()] {
      this_backup->TaskWrapper();
    });
  } else {
    busy_ = false;
  }
}

}  // namespace details

Strand::Strand(IExecutor& underlying)
    : impl_(std::make_shared<details::StrandImpl>(underlying)) {
}

void Strand::Submit(Task task) {
  impl_->Submit(std::move(task));
}

}  // namespace exe::executors
