#pragma once

#include <exe/futures/syntax/pipe.hpp>
#include <exe/futures/combine/seq/flatten.hpp>
#include <exe/futures/combine/seq/map.hpp>

#include <exe/futures/traits/value_of.hpp>

#include <type_traits>

namespace exe::futures {

namespace pipe {

template <typename F>
struct [[nodiscard]] FlatMap {
  F fun;

  explicit FlatMap(F f)
      : fun(std::move(f)) {
  }

  template <typename T>
  using U = traits::ValueOf<std::invoke_result_t<F, T>>;

  template <typename T>
  Future<U<T>> Pipe(Future<T> future) {
    auto [out, promise] = ContractLike<U<T>>(future);
    std::move(future).Consume(
        [p = std::move(promise), f = std::move(fun)](Result<T> in) mutable {
          if (in) {
            f(std::move(in).value())
                .Consume([p = std::move(p)](Result<U<T>> inner_in) mutable {
                  std::move(p).Set(std::move(inner_in));
                });
          } else {
            std::move(p).SetError(std::move(in).error());
          }
        });

    return std::move(out);
  }
};

}  // namespace pipe

// Future<T> -> (T -> Future<U>) -> Future<U>

template <typename F>
auto FlatMap(F fun) {
  return pipe::FlatMap{std::move(fun)};
}

}  // namespace exe::futures
