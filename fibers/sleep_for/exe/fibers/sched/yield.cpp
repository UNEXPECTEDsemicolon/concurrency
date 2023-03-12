#include <exe/fibers/sched/yield.hpp>
#include <exe/fibers/sched/sleep_for.hpp>

namespace exe::fibers {

void Yield() {
  using namespace std::chrono_literals;
  SleepFor(0ms);
}

}  // namespace exe::fibers
