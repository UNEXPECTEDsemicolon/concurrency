#pragma once

#include <exe/result/types/result.hpp>
#include <exe/executors/executor.hpp>
#include <exe/executors/inline.hpp>

#include <exe/futures/state/callback.hpp>
#include <exe/futures/state/rendezvous.hpp>

#include <optional>

namespace exe::futures::detail {

template <typename T>
class SharedState {
 public:
  explicit SharedState(executors::IExecutor& exe)
      : exe_(&exe) {
  }

  void Produce(Result<T> result) {
    result_.emplace(std::move(result));
    if (state_.Produce()) {
      Rendezvous();
    }
  }

  void ProduceNow(Result<T> result) {
    exe_ = &executors::Inline();
    Produce(std::move(result));
  }

  void Consume(detail::Callback<T> cb) {
    cb_.emplace(std::move(cb));
    if (state_.Consume()) {
      Rendezvous();
    }
  }

  void ConsumeNow(detail::Callback<T> cb) {
    exe_ = &executors::Inline();
    Consume(std::move(cb));
  }

  inline executors::IExecutor& GetExecutor() {
    return *exe_;
  }

 private:
  void Rendezvous() {
    exe_->Submit([this]() {
      (*cb_)(std::move(*result_));
      delete this;
    });
  }

  std::optional<Result<T>> result_;
  std::optional<detail::Callback<T>> cb_;
  detail::RendezvousStateMachine state_;
  executors::IExecutor* exe_;
};

}  // namespace exe::futures::detail