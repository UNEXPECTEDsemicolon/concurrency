#include "scheduler.hpp"
#include <twist/ed/local/ptr.hpp>

namespace exe::fibers {

template <typename Executor>
static twist::ed::ThreadLocalPtr<Executor> scheduler = nullptr;

template <typename Executor>
Scheduler<Executor>::Scheduler(Executor& exe)
    : exe_(exe) {
  scheduler<Executor> = this;
}

template <typename Executor>
Executor* Scheduler<Executor>::Current() {
  return scheduler<Executor>;
}

}  // namespace exe::fibers