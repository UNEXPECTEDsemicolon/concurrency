#pragma once

#include <exe/fibers/core/handle.hpp>

namespace exe::fibers {

struct IAwaiter {
  virtual void AwaitSuspend(FiberHandle) = 0;
};

}  // namespace exe::fibers
