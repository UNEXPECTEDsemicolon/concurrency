#pragma once

#include <exe/futures/syntax/pipe.hpp>

#include <exe/result/types/result.hpp>

#include <twist/ed/wait/futex.hpp>
#include <twist/ed/stdlike/atomic.hpp>

namespace exe::futures {

namespace detail {

class Event {
 public:
  void Wait() {
    twist::ed::futex::Wait(ready_, 0);
  }

  void Set() {
    auto wake_key = twist::ed::futex::PrepareWake(ready_);
    ready_.store(1);
    twist::ed::futex::WakeOne(wake_key);
  }

 private:
  twist::ed::stdlike::atomic<uint32_t> ready_{0};
};

}  // namespace detail

namespace pipe {

struct [[nodiscard]] Get {
  template <typename T>
  Result<T> Pipe(Future<T> future) {
    detail::Event event;
    std::optional<Result<T>> out;
    std::move(future).ConsumeNow([&](Result<T> result) {
      out.emplace(std::move(result));
      event.Set();
    });
    event.Wait();
    return std::move(*out);
  }
};

}  // namespace pipe

inline auto Get() {
  return pipe::Get{};
}

}  // namespace exe::futures
