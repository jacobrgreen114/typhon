// Copyright (c) 2023 Jacob R. Green
// All Rights Reserved.

#pragma once

#ifndef __cplusplus
#error
#endif

#include "syntax_tree.hpp"

constexpr auto keyword_auto          = std::string_view{"auto"};

constexpr auto identifer_prefix      = std::string_view{"__ty_"};
constexpr auto namespace_file_prefix = std::string_view{"__ty_ns_"};

enum class GeneratedFile {
  Source,
  HeaderInternal,
  HeaderPublic
};

auto mutate_type_name(std::string_view view) -> std::string_view;

auto get_operator_symbol(Operator op) -> std::string_view;

auto write_include(std::ostream& stream, std::string_view header) -> std::ostream&;