#pragma once

#include <exe/futures/types/future.hpp>

#include <exe/result/types/error.hpp>
#include <exe/futures/make/contract.hpp>

namespace exe::futures {

template <typename T>
Future<T> Failure(Error with) {
  auto [f, p] = Contract<T>();
  std::move(p).SetError(std::move(with));
  return std::move(f);
}

}  // namespace exe::futures
