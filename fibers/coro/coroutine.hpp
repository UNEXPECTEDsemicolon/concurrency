#pragma once

#include <sure/context.hpp>
#include <sure/stack.hpp>

#include <function2/function2.hpp>

#include <exception>

// Simple stackful coroutine

class Coroutine : private sure::ITrampoline {
 public:
  struct CallStack {
    sure::ExecutionContext* routine_ctx;
    CallStack* parent;
  };

  using Routine = fu2::unique_function<void()>;

  explicit Coroutine(Routine routine);

  void Resume();

  // Suspend running coroutine
  static void Suspend();

  bool IsCompleted() const;

 private:
  void Run() noexcept override;

  static const size_t kDefaultStackSize = 64 * 1024;

  Routine routine_;
  sure::Stack routine_stack_;
  sure::ExecutionContext routine_context_;
  sure::ExecutionContext self_context_;
  CallStack grandparent_{.routine_ctx = &self_context_, .parent = nullptr};
  std::exception_ptr exception_;
  bool is_completed_ = false;
};
