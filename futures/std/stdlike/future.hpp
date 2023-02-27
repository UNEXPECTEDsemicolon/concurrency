#pragma once

#include <memory>
#include <cassert>
#include <stdlike/pipe.hpp>

namespace stdlike {

template <typename T>
class Future {
  template <typename U>
  friend class Promise;

 public:
  Future() = delete;

  // Non-copyable
  Future(const Future&) = delete;
  Future& operator=(const Future&) = delete;

  // Movable
  Future(Future&&) = default;
  Future& operator=(Future&&) = default;

  // One-shot
  // Wait for result (value or exception)
  T Get() {
    return pipe_->Get();
  }

 private:
  explicit Future(detail::PipePtr<T> pipe)
      : pipe_(std::move(pipe)) {
  }

  detail::PipePtr<T> pipe_;
};

}  // namespace stdlike
