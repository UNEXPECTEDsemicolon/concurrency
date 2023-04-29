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

  void ProduceVia(Result<T> result, executors::IExecutor& exe) {
    exe_ = &exe;
    return Produce(std::move(result));
  }

  void ProduceNow(Result<T> result) {
    ProduceVia(std::move(result), executors::Inline());
  }

  void Consume(detail::Callback<T> cb) {
    cb_.emplace(std::move(cb));
    if (state_.Consume()) {
      Rendezvous();
    }
  }

  void ConsumeVia(detail::Callback<T> cb, executors::IExecutor& exe) {
    exe_ = &exe;
    return Consume(std::move(cb));
  }

  void ConsumeNow(detail::Callback<T> cb) {
    return ConsumeVia(std::move(cb), executors::Inline());
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