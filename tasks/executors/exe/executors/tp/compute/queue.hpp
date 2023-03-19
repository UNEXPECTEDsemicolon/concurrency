#pragma once

#include <deque>
#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>
#include <optional>

namespace exe::executors::tp::compute::details {

// Unbounded blocking multi-producers/multi-consumers (MPMC) queue

template <typename T>
class UnboundedBlockingQueue {
 public:
  bool Put(T value) {
    std::lock_guard guard(mutex_);
    if (closed_) {
      return false;
    }
    buffer_.push_back(std::move(value));
    not_empty_.notify_one();
    return true;
  }

  std::optional<T> Take() {
    std::unique_lock guard(mutex_);
    not_empty_.wait(guard, [&]() {
      return !buffer_.empty() || closed_;
    });
    if (buffer_.empty() && closed_) {
      return std::nullopt;
    }
    auto result = std::move(buffer_.front());
    buffer_.pop_front();
    return result;
  }

  void Close() {
    std::lock_guard guard(mutex_);
    closed_ = true;
    not_empty_.notify_all();
  }

 private:
  std::deque<T> buffer_;
  twist::ed::stdlike::mutex mutex_;
  twist::ed::stdlike::condition_variable not_empty_;
  bool closed_ = false;
};

}  // namespace exe::executors::tp::compute::details
