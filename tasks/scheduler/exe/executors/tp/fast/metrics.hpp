#pragma once

#include <cstdlib>
#include "fmt/core.h"

namespace exe::executors::tp::fast {

struct WorkerMetrics {
  void Print() const {
    fmt::println("total tasks: {}", tasks_total);
    fmt::println("global tasks: {}", tasts_from_global);
    fmt::println("stealed tasks: {}", tasks_stealed);
    fmt::println("lifo tasks: {}", tasks_from_lifo);
  }

  size_t tasks_stealed = 0;
  size_t tasts_from_global = 0;
  size_t tasks_from_lifo = 0;
  size_t tasks_total = 0;
};

struct PoolMetrics : WorkerMetrics {
  // Your metrics goes here
};

}  // namespace exe::executors::tp::fast
