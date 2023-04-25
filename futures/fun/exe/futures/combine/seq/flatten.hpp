#pragma once

#include <exe/futures/syntax/pipe.hpp>
#include <exe/result/make/err.hpp>

namespace exe::futures {

namespace pipe {

struct [[nodiscard]] Flatten {
  template <typename T>
  Future<T> Pipe(Future<Future<T>> future) {
    auto [out, promise] = ContractLike<T>(future);
    std::move(future).Consume([p = std::move(promise)](
                                  Result<Future<T>> in) mutable {
      if (in) {
        std::move(*in).Consume([p = std::move(p)](Result<T> inner_in) mutable {
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

// Future<Future<T>> -> Future<T>

inline auto Flatten() {
  return pipe::Flatten{};
}

}  // namespace exe::futures
