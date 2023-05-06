#include <exe/executors/tp/fast/worker.hpp>
#include <exe/executors/tp/fast/thread_pool.hpp>

#include <twist/ed/local/ptr.hpp>
#include <utility>
#include "exe/executors/hint.hpp"
#include "exe/executors/task.hpp"
#include "fmt/core.h"
#include "twist/ed/stdlike/random.hpp"
#include "twist/rt/layer/strand/local/ptr.hpp"
#include "wheels/core/defer.hpp"

namespace exe::executors::tp::fast {

TWISTED_THREAD_LOCAL_PTR(Worker, current)

Worker::Worker(ThreadPool& host, size_t index)
    : host_(host),
      index_(index) {
}

void Worker::Start() {
  thread_.emplace([this]() {
    Work();
  });
}

void Worker::Join() {
  thread_->join();
  //   metrics_.Print();
}

void Worker::Push(TaskBase* task, SchedulerHint hint = SchedulerHint::UpToYou) {
  if (host_.stopped_) {
    return;
  }
  if (hint == SchedulerHint::Next) {
    return Push(lifo_slot_.exchange(task));
  }
  while (!local_tasks_.TryPush(task)) {
    TaskBase* overflow[kLocalQueueCapacity];
    auto overflow_size = local_tasks_.Grab({overflow, kLocalQueueCapacity});
    if (overflow_size > 0) {
      host_.global_tasks_.Offload({overflow, overflow_size});
    }
  }
}

TaskBase* Worker::PickTask() {
  // Poll in order:
  // * [%61] Global queue
  // * LIFO slot
  // * Local queue
  // * Global queue
  // * Work stealing
  // Then
  //   Park worker
  while (true) {
    if (host_.stopped_) {
      return nullptr;
    }
    if (++iter_ % 61 == 0) {
      if (auto task = TryGrabTasksFromGlobalQueue()) {
        return task;
      }
    }
    if (iter_ % 7 != 0) {
      if (auto task = TryPickTaskFromLifoSlot()) {
        return task;
      }
    }
    if (auto task = local_tasks_.TryPop()) {
      return task;
    }
    if (auto task = TryGrabTasksFromGlobalQueue()) {
      return task;
    }
    if (auto task = TryStealTasks()) {
      return task;
    }
  }

  return nullptr;
}

TaskBase* Worker::TryPickTaskFromLifoSlot() {
  return lifo_slot_.exchange(nullptr);
}

TaskBase* Worker::TryGrabTasksFromGlobalQueue() {
  auto to_grub = kLocalQueueCapacity - local_tasks_.Size();
  TaskBase* buffer[kLocalQueueCapacity];
  auto grubbed = host_.global_tasks_.Grab({buffer, to_grub});
  if (grubbed > 0) {
    metrics_.tasts_from_global += grubbed;
    local_tasks_.PushMany({buffer + 1, grubbed - 1});
    return buffer[0];
  }
  return nullptr;
}

TaskBase* Worker::TryStealTasks() {
  auto to_grub = kLocalQueueCapacity - local_tasks_.Size();
  auto workers_cnt = host_.workers_.size();
  if (host_.stealers_ > workers_cnt / 2) {
    return nullptr;
  }
  host_.stealers_.fetch_add(1);
  wheels::Defer([this] {
    host_.stealers_.fetch_sub(1);
  });
  TaskBase* buffer[kLocalQueueCapacity];
  twister_.seed(twist::ed::stdlike::random_device()());
  for (size_t victim = twister_() % workers_cnt, iter = 0; iter < workers_cnt;
       victim = twister_() % workers_cnt, ++iter) {
    auto grubbed = host_.workers_[victim].local_tasks_.Grab({buffer, to_grub});
    metrics_.tasks_stealed += grubbed;
    if (grubbed > 0) {
      local_tasks_.PushMany({buffer + 1, grubbed - 1});
      return buffer[0];
    }
  }
  return nullptr;
}

void Worker::Work() {
  current = this;
  host_.SetCurrent(&host_);

  while (TaskBase* next = PickTask()) {
    metrics_.tasks_total++;
    next->Run();
  }
}

Worker* Worker::Current() {
  return current;
}

}  // namespace exe::executors::tp::fast
