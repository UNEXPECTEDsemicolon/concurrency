#pragma once

#include <exe/executors/executor.hpp>

#include <exe/futures/syntax/pipe.hpp>

namespace exe::futures {

namespace pipe {

struct [[nodiscard]] Via {
  executors::IExecutor& executor;

  template <typename T>
  Future<T> Pipe(Future<T> future) {
    auto [f, p] = Contract<T>(executor);
    std::move(future).ConsumeNow([p = std::move(p)](Result<T> in) mutable {
      std::move(p).Set(std::move(in));
    });
    return std::move(f);
  }
};

}  // namespace pipe

// Future<T> -> Executor -> Future<T>

inline auto Via(executors::IExecutor& executor) {
  return pipe::Via{executor};
}

}  // namespace exe::futures
