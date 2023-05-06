#pragma once

namespace exe::executors {

enum class SchedulerHint {
  UpToYou = 1,  // Rely on executor scheduling decision
  Next,         // Use LIFO scheduling
  Global        // Push in global queue
};

}  // namespace exe::executors
