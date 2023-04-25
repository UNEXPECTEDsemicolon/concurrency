#pragma once

#include <exe/futures/types/future.hpp>
#include <twist/ed/stdlike/atomic.hpp>
#include <exe/executors/inline.hpp>

namespace exe::futures {

template <typename... Ts>
Future<std::enable_if_t<std::conjunction_v<std::is_same<
                            std::tuple_element_t<0, std::tuple<Ts>>, Ts>...>,
                        std::tuple_element_t<0, std::tuple<Ts...>>>>
First(Future<Ts>... futures) {
  using T = std::tuple_element_t<0, std::tuple<Ts...>>;
  auto value_set = new twist::ed::stdlike::atomic<bool>(false);
  auto workers = new twist::ed::stdlike::atomic<size_t>(sizeof...(Ts));
  auto state = new detail::SharedState<T>(executors::Inline());

  auto cb = [=](Result<T> in) {
    if (in.has_value() && !(value_set->exchange(true))) {
      state->Produce(std::move(in));
    }
    if (workers->fetch_sub(1) == 1) {
      delete workers;
      delete value_set;
      if (!in.has_value()) {
        state->Produce(std::move(in));
      }
    }
  };

  (std::move(futures).Consume(cb), ...);
  return Future<T>{state};
}

}  // namespace exe::futures
