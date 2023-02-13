#pragma once

#include <memory>
#include <mutex>
#include <twist/ed/stdlike/mutex.hpp>
#include <utility>

//////////////////////////////////////////////////////////////////////

/*
 * Safe API for mutual exclusion
 *
 * Usage:
 *
 * Mutexed<std::vector<Apple>> apples;
 *
 * {
 *   auto owner_ref = apples->Acquire();
 *   owner_ref->push_back(Apple{});
 * }  // <- release ownership
 *
 */

template <typename T, class Mutex = twist::ed::stdlike::mutex>
class Mutexed {
  using NoDeleteUniquePtr = std::unique_ptr<T, decltype([](T*) {})>;
  class OwnerRef : public NoDeleteUniquePtr {
   private:
    explicit OwnerRef(Mutexed<T>& mutexed)
        : NoDeleteUniquePtr(&mutexed.object_),
          guard_(mutexed.mutex_) {
    }

    friend Mutexed<T, Mutex>;

    std::lock_guard<Mutex> guard_;
  };

 public:
  // https://eli.thegreenplace.net/2014/perfect-forwarding-and-universal-references-in-c/
  template <typename... Args>
  explicit Mutexed(Args&&... args)
      : object_(std::forward<Args>(args)...) {
  }

  OwnerRef Acquire() {
    return OwnerRef(*this);
  }

 private:
  T object_;
  Mutex mutex_;  // Guards access to object_
};

//////////////////////////////////////////////////////////////////////

template <typename T>
auto Acquire(Mutexed<T>& object) {
  return object.Acquire();
}
