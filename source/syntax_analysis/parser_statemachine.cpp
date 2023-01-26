// Copyright (c) 2023. Jacob R. Green
// All Rights Reserved.

#include "parser_statemachine.hpp"

#include "parser_def_func.hpp"
#include "parser_def_var.hpp"

#pragma region Global Parser States

constexpr auto error_state = ParserState{
    [](ParserContext& ctx) -> ParserState { throw_not_implemented(); }};

constexpr auto unexpected_token_error_state = ParserState{
    [](ParserContext& ctx) -> ParserState { throw_not_implemented(); }};

constexpr auto exit_state = ParserState{nullptr};

constexpr auto end_state =
    ParserState{[](ParserContext& ctx) -> ParserState { return exit_state; }};

extern const ParserState unknown_state;

auto append_source_node(ParserContext& ctx) -> void {
  auto& node = ctx.syntax_stack.top();
  ctx.source->push_node(node);
  ctx.syntax_stack.pop();
  assert(ctx.syntax_stack.empty());
}

constexpr auto source_node_end_exit_state =
    ParserState{[](ParserContext& ctx) -> ParserState {
      append_source_node(ctx);
      return end_state;
    }};

constexpr auto source_node_end_state =
    ParserState{[](ParserContext& ctx) -> ParserState {
      append_source_node(ctx);
      return unknown_state;
    }};

auto unknown_handler(ParserContext& ctx) -> ParserState {
  auto kind = ctx.current().kind();

  ctx.push_states(source_node_end_state, source_node_end_exit_state);

  switch (kind) {
    case LexicalKind::KeywordVar: {
      return var_def_start_state;
    }
    case LexicalKind::KeywordFunc: {
      return func_def_start_state;
    }
    case LexicalKind::KeywordObject:
      throw_not_implemented();
    default: {
      return unexpected_token_error_state;
    }
  }
}

constexpr ParserState unknown_state = ParserState{unknown_handler};

constexpr auto start_state = ParserState{[](ParserContext& ctx) -> ParserState {
  return ctx.move_next_state(unknown_state, exit_state);
}};

#pragma endregion

#pragma region Parser Context

ParserContext::ParserContext(const std::vector<LexicalToken>& tokens)
    : tokens_{tokens},
      started_{false},
      current_{},
      source{std::make_shared<SourceNode>()} {}

auto ParserContext::current() -> const LexicalToken& { return *current_; }

auto ParserContext::move_next() -> bool {
  if (!started_) {
    current_ = tokens_.begin();
    started_ = true;
    return true;
  }
  return ++current_ < tokens_.end();
}

#pragma endregion

#pragma region Parser

Parser::Parser() : StateMachine<ParserContext>(start_state) {}

#pragma endregion