#pragma once

#include <memory>
#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>
#include <variant>
#include <optional>

namespace stdlike {

namespace detail {

template <typename T>
class Pipe {
 public:
  struct ExceptionPtr : public std::exception_ptr {
    explicit ExceptionPtr(std::exception_ptr ptr)
        : std::exception_ptr(std::move(ptr)) {
    }
  };

  Pipe() = default;

  Pipe(const Pipe&) = delete;
  Pipe& operator=(const Pipe&) = delete;

  Pipe(Pipe&&) = default;
  Pipe& operator=(Pipe&&) = default;

  T Get() {
    std::unique_lock guard(mutex_);
    if (is_taken_) {
      throw std::runtime_error("Multiple Get is not possible");
    }
    waiter_.wait(guard, [&]() {
      return data_.has_value();
    });
    is_taken_ = true;
    if (data_->index() == 0) {
      return std::get<0>(std::move(*data_));
    } else {
      std::rethrow_exception(std::get<1>(std::move(*data_)));
    }
  }

  void Set(auto value_or_ex) {
    std::unique_lock guard(mutex_);
    if (is_set_) {
      throw std::runtime_error("Multiple Set is not possible");
    }
    is_set_ = true;
    data_ = std::move(value_or_ex);
    waiter_.notify_one();
  }

 private:
  std::optional<std::variant<T, ExceptionPtr>> data_;
  twist::ed::stdlike::mutex mutex_;
  twist::ed::stdlike::condition_variable waiter_;
  bool is_taken_ = false;
  bool is_set_ = false;
};

template <typename T>
class PipePtr : public std::shared_ptr<Pipe<T>> {
 public:
  PipePtr()
      : std::shared_ptr<Pipe<T>>::shared_ptr(std::make_shared<Pipe<T>>()) {
  }
};

}  // namespace detail
}  // namespace stdlike