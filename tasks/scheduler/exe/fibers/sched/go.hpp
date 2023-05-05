#pragma once

#include <exe/fibers/core/routine.hpp>
#include "exe/executors/executor.hpp"

namespace exe::fibers {

// Considered harmful

void Go(executors::IExecutor&, Routine);

void Go(Routine);

}  // namespace exe::fibers
