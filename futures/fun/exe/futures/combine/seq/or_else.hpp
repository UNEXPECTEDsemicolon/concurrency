#pragma once

#include <exe/futures/syntax/pipe.hpp>

namespace exe::futures {

namespace pipe {

template <typename F>
struct [[nodiscard]] OrElse {
  F fun;

  explicit OrElse(F f)
      : fun(std::move(f)) {
  }

  template <typename T>
  Future<T> Pipe(Future<T> future) {
    auto [out, promise] = ContractLike<T>(future);
    std::move(future).Consume(
        [p = std::move(promise), f = std::move(fun)](Result<T> in) mutable {
          if (in) {
            std::move(p).Set(std::move(in));
          } else {
            std::move(p).Set(f(std::move(in).error()));
          }
        });
    return std::move(out);
  }
};

}  // namespace pipe

// Future<T> -> (Error -> Result<T>) -> Future<T>

template <typename F>
auto OrElse(F fun) {
  return pipe::OrElse{std::move(fun)};
}

}  // namespace exe::futures
