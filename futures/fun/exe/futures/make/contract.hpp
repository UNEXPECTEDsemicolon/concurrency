#pragma once

#include <exe/futures/types/future.hpp>
#include <exe/futures/state/shared_state.hpp>
#include <exe/result/types/result.hpp>
#include <exe/result/make/err.hpp>
#include <exe/result/make/ok.hpp>

#include <exe/executors/executor.hpp>
#include <exe/executors/inline.hpp>

#include <tuple>

namespace exe::futures {

template <typename T>
class Promise {
  using State = detail::SharedState<T>;

  template <typename U>
  friend std::tuple<Future<U>, Promise<U>> Contract(executors::IExecutor&);

 public:
  Promise(const Promise&) = delete;
  Promise& operator=(const Promise&) = delete;

  Promise(Promise&& source)
      : state_(source.Release()) {
  }

  void Set(Result<T> result) && {
    Release()->Produce(std::move(result));
  }

  void SetNow(Result<T> result) && {
    Release()->ProduceNow(std::move(result));
  }

  void SetValue(T value) && {
    std::move(*this).Set(result::Ok(std::move(value)));
  }

  void SetError(Error err) && {
    std::move(*this).Set(result::Err(std::move(err)));
  }

 private:
  explicit Promise(State* state)
      : state_(state) {
  }

  State* Release() {
    return std::exchange(state_, nullptr);
  }

  State* state_;
};

template <typename T>
std::tuple<Future<T>, Promise<T>> Contract(
    executors::IExecutor& exe = executors::Inline()) {
  auto state = new detail::SharedState<T>(exe);
  return {Future<T>{state}, Promise<T>{state}};
}

template <typename T, typename U>
inline std::tuple<Future<T>, Promise<T>> ContractLike(Future<U>& src) {
  return Contract<T>(src.GetExecutor());
}

}  // namespace exe::futures
