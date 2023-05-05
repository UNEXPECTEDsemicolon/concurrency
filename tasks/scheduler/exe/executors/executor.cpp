#include "executor.hpp"
#include <twist/ed/local/ptr.hpp>

namespace exe::executors {

static twist::ed::ThreadLocalPtr<IExecutor> current = nullptr;

void IExecutor::SetCurrent(IExecutor* exe) {
  current = exe;
}

IExecutor* IExecutor::Current() {
  return current;
}

}  // namespace exe::executors