#include <exe/coro/core.hpp>

#include <twist/ed/local/ptr.hpp>

#include <wheels/core/assert.hpp>
#include <wheels/core/compiler.hpp>

namespace exe::coro {

static twist::ed::ThreadLocalPtr<Coroutine::CallStack> co_callstack;

Coroutine::Coroutine(Routine routine)
    : routine_(std::move(routine)),
      routine_stack_(sure::Stack::AllocateBytes(kDefaultStackSize)) {
  routine_context_.Setup(routine_stack_.MutView(), this);
}

void Coroutine::Resume() {
  if (co_callstack == nullptr) {
    co_callstack = &grandparent_;
  }
  CallStack new_call{.routine_ctx = &routine_context_, .parent = co_callstack};
  co_callstack = &new_call;
  co_callstack->parent->routine_ctx->SwitchTo(routine_context_);
  if (exception_) {
    std::rethrow_exception(exception_);
  }
}

void Coroutine::Suspend() {
  auto cur_ctx = co_callstack->routine_ctx;
  co_callstack = co_callstack->parent;
  cur_ctx->SwitchTo(*co_callstack->routine_ctx);
}

bool Coroutine::IsCompleted() const {
  return is_completed_;
}

void Coroutine::Run() noexcept {
  try {
    routine_();
  } catch (...) {
    exception_ = std::current_exception();
  }
  is_completed_ = true;
  auto cur_call = *co_callstack;
  co_callstack = co_callstack->parent;
  if (co_callstack == &grandparent_) {
    co_callstack = nullptr;
  }
  cur_call.routine_ctx->ExitTo(*cur_call.parent->routine_ctx);
}

twist::ed::ThreadLocalPtr<Coroutine::CallStack>& Coroutine::GetCallstack() {
  return co_callstack;
}

}  // namespace exe::coro
