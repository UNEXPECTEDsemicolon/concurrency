#pragma once

#include <exe/futures/state/shared_state.hpp>
#include <exe/executors/executor.hpp>

namespace exe::futures {

template <typename T>
class Promise;

template <typename T>
struct [[nodiscard]] Future {
  using ValueType = T;

  // Non-copyable
  Future(const Future&) = delete;
  Future& operator=(const Future&) = delete;

  // Non-copy-assignable
  Future& operator=(Future&) = delete;

  // Movable
  Future(Future&& source)
      : state_(source.Release()) {
  }

  void Consume(detail::Callback<T> cb) && {
    Release()->Consume(std::move(cb));
  }

  void ConsumeNow(detail::Callback<T> cb) && {
    Release()->ConsumeNow(std::move(cb));
  }

  inline executors::IExecutor& GetExecutor() {
    return state_->GetExecutor();
  }

  using State = detail::SharedState<T>;

  State* Release() {
    return std::exchange(state_, nullptr);
  }

  Future(State* state)  // NOLINT
      : state_(state) {
  }

 private:
  State* state_;
};

}  // namespace exe::futures
