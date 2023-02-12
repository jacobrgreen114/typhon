// Copyright (c) 2023 Jacob R. Green
// All Rights Reserved.

#pragma once

#ifndef __cplusplus
#error
#endif

#include "token.hpp"
#include "state_machine.hpp"

#include "syntax_tree.hpp"

class ParserContext;
using ParserState = State<ParserContext>;

struct ReturnState {
  ParserState ret;
  ParserState end;
};

class ParserContext final : public EnumeratingContext<ParserContext, LexicalToken> {
 public:
  using Source          = std::unique_ptr<SyntaxTree>;
  using StateStack      = std::stack<ReturnState>;
  using SyntaxStack     = std::stack<std::unique_ptr<SyntaxNode>>;
  using PrecedenceStack = std::stack<Precedence>;
  using TokenStack      = std::stack<LexicalToken>;

 private:
  const TokenCollection tokens_;
  bool started_;
  std::vector<LexicalToken>::const_iterator current_;

  StateStack state_stack;

 public:
  // todo : privitize fields

  Source source;

  SyntaxStack syntax_stack;
  PrecedenceStack precedence_stack;
  TokenStack token_stack;

  explicit ParserContext(const TokenCollection& tokens);
  virtual ~ParserContext() = default;

  auto current() -> const LexicalToken& override;
  auto move_next() -> bool override;

  auto push_states(ParserState ret_state, ParserState end_state) {
    state_stack.push({ret_state, end_state});
  }

 private:
  auto pop_states() {
    auto state = state_stack.top();
    state_stack.pop();
    return state;
  }

 public:
  auto push_current_token() -> void { token_stack.emplace(current()); }

  auto pop_ret_state() -> ParserState { return pop_states().ret; }

  auto pop_end_state() -> ParserState { return pop_states().end; }

  auto move_next_stack() -> ParserState {
    auto states = pop_states();
    return move_next_state(states.ret, states.end);
  }

  template <IsSyntaxNode T>
  auto get_syntax_node() const -> T* {
    auto* ptr = dynamic_cast<T*>(syntax_stack.top().get());
    assert(ptr);
    return ptr;
  }

  template <IsSyntaxNode T>
  auto pop_syntax_node() -> std::unique_ptr<T> {
    auto tnode = ptr_cast<T>(std::move(syntax_stack.top()));
    syntax_stack.pop();
    return tnode;
  }

  auto get_var_def_node() { return get_syntax_node<VarDefinition>(); }
  auto pop_def_node() { return pop_syntax_node<Definition>(); }

  auto get_func_def_node() { return get_syntax_node<FuncDefinition>(); }
  auto get_func_param_node() { return get_syntax_node<FuncParameter>(); }
  auto pop_func_def_param() { return pop_syntax_node<FuncParameter>(); }

  auto get_statement_node() { return get_syntax_node<Statement>(); }
  auto pop_statement_node() { return pop_syntax_node<Statement>(); }

  auto get_statement_def_node() { return get_syntax_node<DefStatement>(); }
  auto get_statement_expr_node() { return get_syntax_node<ExprStatement>(); }

  auto get_statement_return_node() { return get_syntax_node<ReturnStatement>(); }

  auto get_statement_block() { return get_syntax_node<StatementBlock>(); }
  auto pop_statement_block() { return pop_syntax_node<StatementBlock>(); }

  auto get_statement_if_node() { return get_syntax_node<IfStatement>(); }
  auto get_statement_elif_node() { return get_syntax_node<ElifStatement>(); }
  auto get_statement_else_node() { return get_syntax_node<ElseStatement>(); }

  auto pop_expr_node() { return pop_syntax_node<ExpressionNode>(); }

  auto get_expr_unary_node() { return get_syntax_node<UnaryExpression>(); }

  auto pop_expr_binary_node() { return pop_syntax_node<BinaryExpression>(); }
};

using ParserMatchCondition = ParserContext::RefMatchCondition;

#pragma endregion

class Parser final : public StateMachine<ParserContext> {
 public:
  Parser();
};

using LexicalTokenPredicate = Predicate<const LexicalToken&>;

constexpr auto is_token_kind(const LexicalToken& token, const LexicalKind kind) -> bool {
  return token.kind() == kind;
};

constexpr LexicalTokenPredicate is_identifier = [](auto& token) {
  return is_token_kind(token, LexicalKind::Identifier);
};

constexpr LexicalTokenPredicate is_semicolon = [](auto& token) {
  return is_token_kind(token, LexicalKind::SymbolSemicolon);
};

constexpr LexicalTokenPredicate is_colon = [](auto& token) {
  return is_token_kind(token, LexicalKind::SymbolColon);
};

constexpr LexicalTokenPredicate is_doublecolon = [](auto& token) {
  return is_token_kind(token, LexicalKind::SymbolDoubleColon);
};

constexpr LexicalTokenPredicate is_comma = [](auto& token) {
  return is_token_kind(token, LexicalKind::SymbolComma);
};

constexpr LexicalTokenPredicate is_paren_open = [](auto& token) {
  return is_token_kind(token, LexicalKind::SymbolParenOpen);
};

constexpr LexicalTokenPredicate is_paren_close = [](auto& token) {
  return is_token_kind(token, LexicalKind::SymbolParenClose);
};

constexpr LexicalTokenPredicate is_square_open = [](auto& token) {
  return is_token_kind(token, LexicalKind::SymbolsquareOpen);
};

constexpr LexicalTokenPredicate is_square_close = [](auto& token) {
  return is_token_kind(token, LexicalKind::SymbolsquareClose);
};

constexpr LexicalTokenPredicate is_curly_open = [](auto& token) {
  return is_token_kind(token, LexicalKind::SymbolCurlyOpen);
};

constexpr LexicalTokenPredicate is_curly_close = [](auto& token) {
  return is_token_kind(token, LexicalKind::SymbolCurlyClose);
};

constexpr LexicalTokenPredicate is_arrow = [](auto& token) {
  return is_token_kind(token, LexicalKind::SymbolArrow);
};

constexpr LexicalTokenPredicate is_equals = [](auto& token) {
  return is_token_kind(token, LexicalKind::SymbolEquals);
};

constexpr LexicalTokenPredicate is_keyword_if = [](auto& token) {
  return is_token_kind(token, LexicalKind::KeywordIf);
};

constexpr LexicalTokenPredicate is_keyword_elif = [](auto& token) {
  return is_token_kind(token, LexicalKind::KeywordElif);
};

constexpr LexicalTokenPredicate is_keyword_else = [](auto& token) {
  return is_token_kind(token, LexicalKind::KeywordElse);
};

constexpr LexicalTokenPredicate is_keyword_loop = [](auto& token) {
  return is_token_kind(token, LexicalKind::KeywordLoop);
};

constexpr LexicalTokenPredicate is_keyword_while = [](auto& token) {
  return is_token_kind(token, LexicalKind::KeywordWhile);
};

constexpr LexicalTokenPredicate is_keyword_for = [](auto& token) {
  return is_token_kind(token, LexicalKind::KeywordFor);
};

constexpr LexicalTokenPredicate is_keyword_foreach = [](auto& token) {
  return is_token_kind(token, LexicalKind::KeywordForeach);
};

constexpr LexicalTokenPredicate is_keyword_var = [](auto& token) {
  return is_token_kind(token, LexicalKind::KeywordVar);
};

constexpr LexicalTokenPredicate is_keyword_func = [](auto& token) {
  return is_token_kind(token, LexicalKind::KeywordFunc);
};

constexpr LexicalTokenPredicate is_keyword_struct = [](auto& token) {
  return is_token_kind(token, LexicalKind::KeywordStruct);
};

constexpr LexicalTokenPredicate is_keyword_object = [](auto& token) {
  return is_token_kind(token, LexicalKind::KeywordObject);
};

constexpr LexicalTokenPredicate is_keyword_return = [](auto& token) {
  return is_token_kind(token, LexicalKind::KeywordReturn);
};

auto is_unary_pre_operator(const LexicalToken& token) -> bool;
auto is_unary_post_operator(const LexicalToken& token) -> bool;

auto is_binary_operator(const LexicalToken& token) -> bool;

extern const ParserState error_state;
extern const ParserState unexpected_token_error_state;