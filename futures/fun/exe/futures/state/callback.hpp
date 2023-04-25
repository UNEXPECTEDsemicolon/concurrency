#pragma once

#include <function2/function2.hpp>
#include <exe/result/types/result.hpp>

namespace exe::futures::detail {

template <typename T>
using Callback = fu2::unique_function<void(Result<T>)>;

}  // namespace exe::futures::detail
