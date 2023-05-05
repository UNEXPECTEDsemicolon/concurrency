#pragma once

#include <deque>
#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>
#include <optional>
#include "wheels/intrusive/forward_list.hpp"

namespace exe::executors::tp::compute::details {

// Unbounded blocking multi-producers/multi-consumers (MPMC) queue

template <typename T>
class UnboundedBlockingQueue {
 public:
  bool Put(T* node) {
    std::lock_guard guard(mutex_);
    if (closed_) {
      return false;
    }
    buffer_.PushBack(node);
    not_empty_.notify_one();
    return true;
  }

  std::optional<T*> Take() {
    std::unique_lock guard(mutex_);
    not_empty_.wait(guard, [&]() {
      return !buffer_.IsEmpty() || closed_;
    });
    if (buffer_.IsEmpty() && closed_) {
      return std::nullopt;
    }
    return buffer_.PopFront();
  }

  void Close() {
    std::lock_guard guard(mutex_);
    closed_ = true;
    not_empty_.notify_all();
  }

 private:
  wheels::IntrusiveForwardList<T> buffer_;
  // std::deque<T> buffer_;
  twist::ed::stdlike::mutex mutex_;
  twist::ed::stdlike::condition_variable not_empty_;
  bool closed_ = false;
};

}  // namespace exe::executors::tp::compute::details
