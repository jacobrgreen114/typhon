
// Copyright (c) 2023 Jacob R. Green
// All Rights Reserved.

#pragma once

#ifndef __cplusplus
#error
#endif

#include <array>
#include <exception>
#include <fstream>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>
#include <filesystem>
#include <iostream>
#include <span>
#include <chrono>
#include <unordered_set>
#include <set>
#include <cassert>

namespace fs     = std::filesystem;
namespace chrono = std::chrono;

#ifndef NODISCARD
#define NODISCARD [[nodiscard]]
#endif

#define throw_not_implemented() throw std::exception("not implemented!")

#ifndef NDEBUG
template <typename To, typename From>
constexpr auto ptr_cast(std::unique_ptr<From> from) -> std::unique_ptr<To> {
  if (!from) {
    return nullptr;
  }
  auto* temp = dynamic_cast<To*>(from.release());
  assert(temp != nullptr);
  return std::unique_ptr<To>(temp);
}

template <typename To, typename From>
constexpr auto ptr_cast(From* from) -> To* {
  if (from == nullptr) {
    return nullptr;
  }
  auto* temp = dynamic_cast<To*>(from);
  assert(temp != nullptr);
  return temp;
}

#else
template <typename To, typename From>
constexpr auto ptr_cast(std::unique_ptr<From> from) -> std::unique_ptr<To> {
  return std::unique_ptr<To>(reinterpret_cast<To*>(from.release()));
}

template <typename To, typename From>
constexpr auto ptr_cast(From* from) -> To* {
  return reinterpret_cast<To*>(from);
}
#endif

inline std::ostream& newline(std::ostream& os) {
  os.put(os.widen('\n'));
  return os;
}

inline std::ostream& newline(std::ostream& os, uint64_t count) {
  for (auto i = 0; i < count; ++i) {
    os.put(os.widen('\n'));
  }
  return os;
}

#ifdef TRACE
#define TRACE_PRINT(str) std::cout << "[TRACE] " << str
#else
#define TRACE_PRINT(str) ((void)0)
#endif
