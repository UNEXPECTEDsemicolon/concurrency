#pragma once

#include <exe/futures/types/future.hpp>
#include <exe/result/types/unit.hpp>
#include <exe/futures/make/value.hpp>

namespace exe::futures {

inline Future<Unit> Just() {
  return Value(Unit());
}

}  // namespace exe::futures
