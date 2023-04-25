#pragma once

#include <twist/ed/stdlike/atomic.hpp>

namespace exe::futures::detail {

class RendezvousStateMachine {
  enum State : int {
    Init = 0,
    Producer = 1,
    Consumer = 2,
    Rendezvous = Producer | Consumer,
  };

 public:
  bool Produce() {
    return state_.fetch_or(State::Producer) == State::Consumer;
  }

  bool Consume() {
    return state_.fetch_or(State::Consumer) == State::Producer;
  }

 private:
  twist::ed::stdlike::atomic<int> state_{State::Init};
};
}  // namespace exe::futures::detail
