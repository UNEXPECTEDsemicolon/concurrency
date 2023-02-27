#pragma once

#include <stdlike/future.hpp>
#include <stdlike/pipe.hpp>

#include <memory>

namespace stdlike {

template <typename T>
class Promise {
 public:
  Promise() = default;

  // Non-copyable
  Promise(const Promise&) = delete;
  Promise& operator=(const Promise&) = delete;

  // Movable
  Promise(Promise&&) = default;
  Promise& operator=(Promise&&) = default;

  // One-shot
  Future<T> MakeFuture() {
    if (future_made_) {
      throw std::runtime_error("Multiple MakeFuture is not possible");
    }
    future_made_ = true;
    return Future<T>(pipe_);
  }

  // One-shot
  // Fulfill promise with value
  void SetValue(T value) {
    pipe_->Set(std::move(value));
  }

  // One-shot
  // Fulfill promise with exception
  void SetException(std::exception_ptr exception) {
    pipe_->Set(typename detail::Pipe<T>::ExceptionPtr(std::move(exception)));
  }

 private:
  detail::PipePtr<T> pipe_;
  bool future_made_ = false;
};

}  // namespace stdlike
