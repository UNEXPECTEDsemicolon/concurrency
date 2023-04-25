#pragma once

#include <exe/futures/types/future.hpp>
#include <exe/result/types/result.hpp>
#include <exe/result/make/err.hpp>

#include <optional>
#include <tuple>

namespace exe::futures {

namespace detail {

template <typename... Types, size_t... inds>
Future<std::tuple<Types...>> AllImpl(std::index_sequence<inds...>,
                                     std::tuple<Future<Types>...> futures) {
  using T = std::tuple<Types...>;
  auto res = new std::tuple<std::optional<Result<Types>>...>;
  auto workers = new twist::ed::stdlike::atomic<size_t>(sizeof...(Types));
  auto succeeded = new twist::ed::stdlike::atomic<size_t>(0);
  auto state = new detail::SharedState<T>(executors::Inline());

  (std::move(std::get<inds>(futures)).Consume([=](Result<Types> in) {
    std::get<inds>(*res).emplace(std::move(in));
    if (std::get<inds>(*res)->has_value()) {
      if (succeeded->fetch_add(1) == sizeof...(Types) - 1) {
        state->Produce(
            Result<T>(std::make_tuple(std::move(**std::get<inds>(*res))...)));
      }
    } else {
      state->Produce(result::Err(std::move(in).error()));
    }
    if (workers->fetch_sub(1) == 1) {
      delete res;
      delete workers;
      delete succeeded;
    }
  }),
   ...);

  return Future<T>{state};
}

}  // namespace detail

template <typename... Types>
Future<std::tuple<Types...>> All(Future<Types>... futures) {
  return detail::AllImpl(std::index_sequence_for<Types...>{},
                         std::make_tuple(std::move(futures)...));
}

template <typename X, typename Y>
Future<std::tuple<X, Y>> Both(Future<X> future1, Future<Y> future2) {
  return All(std::move(future1), std::move(future2));
}

}  // namespace exe::futures
