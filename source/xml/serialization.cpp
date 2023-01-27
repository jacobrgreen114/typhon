// Copyright (c) 2023. Jacob R. Green
// All Rights Reserved.

#include "serialization.hpp"

namespace xml {

SerializationContext::SerializationContext(std::ostream& writer,
                                           const char* name, size_t indent)
    : writer_{writer},
      element_name_{name},
      indent_{indent},
      attr_end_{false},
      element_count_{0} {
  write_indent();
  writer_ << '<' << name;
}

SerializationContext::~SerializationContext() {
  terminate_open();
  if (element_count_ > 0) {
    write_indent();
  }
  writer_ << '<' << '/' << element_name_ << '>' << std::endl;
}

auto SerializationContext::write_indent() -> void {
  for (size_t i = 0; i < indent_; ++i) {
    writer_ << '\t';
  }
}

auto SerializationContext::terminate_open() -> void {
  if (!attr_end_) {
    attr_end_ = true;
    writer_ << '>';
  }
}

auto SerializationContext::add_attribute(const char* name, const char* value)
    -> void {
  if (attr_end_) {
    throw std::exception();
  }
  writer_ << ' ' << name << '=' << '"' << value << '"';
}

auto SerializationContext::add_content(const char* value) -> void {
  terminate_open();
  if (element_count_ > 0) {
    throw std::exception();
  }
  writer_ << value;
}

auto SerializationContext::add_content(uintptr_t value) -> void {
  terminate_open();
  if (element_count_ > 0) {
    throw std::exception();
  }
  writer_ << value;
}
auto SerializationContext::add_content(intptr_t value) -> void {
  terminate_open();
  if (element_count_ > 0) {
    throw std::exception();
  }
  writer_ << value;
}

auto SerializationContext::add_element(const char* name,
                                       const Serializable& serializable)
    -> void {
  terminate_open();
  if (element_count_++ < 1) {
    writer_ << std::endl;
  }
  auto context = SerializationContext{writer_, name, indent_ + 1};
  serializable.on_serialize(context);
}

auto Serializable::on_serialize(SerializationContext& context) const -> void {}

auto Serializable::serialize(std::ostream& writer, const char* root_name) const
    -> void {
  auto context = SerializationContext{writer, root_name};
  on_serialize(context);
  writer.flush();
}

}  // namespace xml