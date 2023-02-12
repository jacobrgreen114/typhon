// Copyright (c) 2023 Jacob R. Green
// All Rights Reserved.

#pragma once

#ifndef __cplusplus
#error
#endif  //__cplusplus

#include <string>
#include <memory>
#include <vector>

#include "token.hpp"
#include "xml/serialization.hpp"

/*
 * Syntax Kind
 */

enum class SyntaxType {
  Misc,
  Expression,
  Statement,
  Definition
};

using syntax_kind_t                     = uint32_t;
constexpr auto syntax_kind_type_offset  = 24;
constexpr auto syntax_kind_type_bitmask = 0xff;
constexpr auto syntax_kind_type_mask    = syntax_kind_type_bitmask << syntax_kind_type_offset;

constexpr auto make_syntax_kind(SyntaxType type, syntax_kind_t value) -> syntax_kind_t {
  return (static_cast<std::underlying_type_t<SyntaxType>>(type) << syntax_kind_type_offset) | value;
}

enum class SyntaxKind : syntax_kind_t {
  Unknown        = 0,

  Source         = make_syntax_kind(SyntaxType::Misc, 1),
  Block          = make_syntax_kind(SyntaxType::Misc, 2),
  Namespace      = make_syntax_kind(SyntaxType::Misc, 0x11),
  Import         = make_syntax_kind(SyntaxType::Misc, 0x12),

  ExprBool       = make_syntax_kind(SyntaxType::Expression, 0x01),
  ExprNumber     = make_syntax_kind(SyntaxType::Expression, 0x02),
  ExprString     = make_syntax_kind(SyntaxType::Expression, 0x03),
  ExprIdentifier = make_syntax_kind(SyntaxType::Expression, 0x04),
  ExprCall       = make_syntax_kind(SyntaxType::Expression, 0x05),

  ExprUnary      = make_syntax_kind(SyntaxType::Expression, 0x11),
  ExprBinary     = make_syntax_kind(SyntaxType::Expression, 0x12),
  ExprTernary    = make_syntax_kind(SyntaxType::Expression, 0x13),

  StmtDef        = make_syntax_kind(SyntaxType::Statement, 0x01),
  StmtExpr       = make_syntax_kind(SyntaxType::Statement, 0x02),
  StmtRet        = make_syntax_kind(SyntaxType::Statement, 0x03),
  StmtIf         = make_syntax_kind(SyntaxType::Statement, 0x11),
  StmtElif       = make_syntax_kind(SyntaxType::Statement, 0x12),
  StmtElse       = make_syntax_kind(SyntaxType::Statement, 0x13),
  StmtLoop       = make_syntax_kind(SyntaxType::Statement, 0x21),
  StmtWhile      = make_syntax_kind(SyntaxType::Statement, 0x22),
  StmtFor        = make_syntax_kind(SyntaxType::Statement, 0x23),
  StmtForeach    = make_syntax_kind(SyntaxType::Statement, 0x24),

  DefVar         = make_syntax_kind(SyntaxType::Definition, 0x01),
  DefFunc        = make_syntax_kind(SyntaxType::Definition, 0x02),
  DefParam       = make_syntax_kind(SyntaxType::Definition, 0x03),
  DefStruct      = make_syntax_kind(SyntaxType::Definition, 0x04),
  DefObject      = make_syntax_kind(SyntaxType::Definition, 0x05),
  DefInterface   = make_syntax_kind(SyntaxType::Definition, 0x06),
};

auto to_string(SyntaxKind kind) -> std::string_view;

constexpr auto get_syntax_type(SyntaxKind kind) -> SyntaxType {
  return static_cast<SyntaxType>(
      (static_cast<std::underlying_type_t<SyntaxKind>>(kind) & syntax_kind_type_mask) >>
      syntax_kind_type_offset);
}

constexpr auto is_expression(SyntaxKind kind) -> bool {
  return get_syntax_type(kind) == SyntaxType::Expression;
}

constexpr auto is_statement(SyntaxKind kind) -> bool {
  return get_syntax_type(kind) == SyntaxType::Statement;
}

constexpr auto is_definition(SyntaxKind kind) -> bool {
  return get_syntax_type(kind) == SyntaxType::Definition;
}

/*
 * Operator
 */

enum class Precedence {
  Assignment = 1,
  BoolOr,
  BoolAnd,
  BitOr,
  BitXor,
  BitAnd,
  Equality,
  Relation,
  Shift,
  AddSub,
  MulDiv,
  Prefix,
  Access,
  Postfix,
};

enum class OperatorType {
  Unary   = 1,
  Binary  = 2,
  Ternary = 3
};

using operator_t                     = uint32_t;

constexpr auto operator_type_offset  = 30;
constexpr auto operator_type_bitmask = 0b11;
constexpr auto operator_type_mask    = operator_type_bitmask << operator_type_offset;

constexpr auto operator_prec_offset  = 24;
constexpr auto operator_prec_bitmask = 0b111111;
constexpr auto operator_prec_mask    = operator_prec_bitmask << operator_prec_offset;

consteval auto make_operator_value(OperatorType type,
                                   Precedence precedence,
                                   operator_t base_value = 0) -> operator_t {
  return (static_cast<operator_t>(type) << operator_type_offset) |
         (static_cast<operator_t>(precedence) << operator_prec_offset) | base_value;
}

// todo : bake symbol into operator
enum class Operator : operator_t {
  // Binary

  Static            = make_operator_value(OperatorType::Binary, Precedence::Access, 0x01),
  Access            = make_operator_value(OperatorType::Binary, Precedence::Access, 0x02),

  Assign            = make_operator_value(OperatorType::Binary, Precedence::Assignment, 0x01),

  SelfAdd           = make_operator_value(OperatorType::Binary, Precedence::Assignment, 0x11),
  SelfSub           = make_operator_value(OperatorType::Binary, Precedence::Assignment, 0x12),
  SelfMul           = make_operator_value(OperatorType::Binary, Precedence::Assignment, 0x13),
  SelfDiv           = make_operator_value(OperatorType::Binary, Precedence::Assignment, 0x14),

  SelfBitOr         = make_operator_value(OperatorType::Binary, Precedence::Assignment, 0x21),
  SelfBitXor        = make_operator_value(OperatorType::Binary, Precedence::Assignment, 0x22),
  SelfBitAnd        = make_operator_value(OperatorType::Binary, Precedence::Assignment, 0x23),

  Add               = make_operator_value(OperatorType::Binary, Precedence::AddSub, 0),
  Subtract          = make_operator_value(OperatorType::Binary, Precedence::AddSub, 1),
  Multiply          = make_operator_value(OperatorType::Binary, Precedence::MulDiv, 0),
  Divide            = make_operator_value(OperatorType::Binary, Precedence::MulDiv, 1),

  Equals            = make_operator_value(OperatorType::Binary, Precedence::Equality, 0),
  NotEquals         = make_operator_value(OperatorType::Binary, Precedence::Equality, 1),
  Or                = make_operator_value(OperatorType::Binary, Precedence::BoolOr, 0),
  And               = make_operator_value(OperatorType::Binary, Precedence::BoolAnd, 0),

  LessThan          = make_operator_value(OperatorType::Binary, Precedence::Relation, 0),
  GreaterThan       = make_operator_value(OperatorType::Binary, Precedence::Relation, 1),
  LessThanEquals    = make_operator_value(OperatorType::Binary, Precedence::Relation, 2),
  GreaterThanEquals = make_operator_value(OperatorType::Binary, Precedence::Relation, 3),

  BitOr             = make_operator_value(OperatorType::Binary, Precedence::BitOr, 0),
  BitXor            = make_operator_value(OperatorType::Binary, Precedence::BitXor, 0),
  BitAnd            = make_operator_value(OperatorType::Binary, Precedence::BitAnd, 0),

  ShiftLeft         = make_operator_value(OperatorType::Binary, Precedence::Shift, 0),
  ShiftRight        = make_operator_value(OperatorType::Binary, Precedence::Shift, 1),

  // Unary
  BoolNot           = make_operator_value(OperatorType::Unary, Precedence::Prefix, 0),
  BitNot            = make_operator_value(OperatorType::Unary, Precedence::Prefix, 1),

  Positive          = make_operator_value(OperatorType::Unary, Precedence::Prefix, 2),
  Negative          = make_operator_value(OperatorType::Unary, Precedence::Prefix, 3),

  PreInc            = make_operator_value(OperatorType::Unary, Precedence::Prefix, 4),
  PreDec            = make_operator_value(OperatorType::Unary, Precedence::Prefix, 5),

  PostInc           = make_operator_value(OperatorType::Unary, Precedence::Postfix, 0),
  PostDec           = make_operator_value(OperatorType::Unary, Precedence::Postfix, 1),
};

auto to_string(Operator op) -> std::string_view;

constexpr auto get_operator_type(Operator op) -> OperatorType {
  return static_cast<OperatorType>(
      (static_cast<std::underlying_type_t<Operator>>(op) & operator_type_mask) >>
      operator_type_offset);
};

constexpr auto get_precedence(Operator op) -> Precedence {
  return static_cast<Precedence>(
      (static_cast<std::underlying_type_t<Operator>>(op) & operator_prec_mask) >>
      operator_prec_offset);
}

auto get_unary_pre_op(LexicalKind kind) -> Operator;

auto get_binary_op(LexicalKind kind) -> Operator;

/*
 * Modifiers
 */

enum class AccessModifier : uint8_t {
  Private,
  Module,
  Internal,
  Protected,
  Public
};

auto to_string(AccessModifier modifier) -> std::string_view;

/*
 * Syntax Tree
 */

class SyntaxNode
#ifdef TRACE
    : public xml::Serializable
#endif
{
 private:
  const SyntaxKind kind_;

 protected:
  explicit constexpr SyntaxNode(const SyntaxKind kind)
      : kind_{kind} {}

 public:
  virtual ~SyntaxNode() = default;

  constexpr auto kind() const noexcept { return kind_; }

 protected:
#ifdef TRACE
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/*
 * Expressions
 */

class ExpressionNode : public SyntaxNode {
 protected:
  explicit constexpr ExpressionNode(const SyntaxKind kind)
      : SyntaxNode{kind} {}
};

class ConstantExpression : public ExpressionNode {
 protected:
  constexpr explicit ConstantExpression(SyntaxKind kind)
      : ExpressionNode{kind} {}
};

class BoolExpression final : public ConstantExpression {
  bool value_;

 public:
  constexpr explicit BoolExpression(bool value)
      : ConstantExpression{SyntaxKind::ExprBool},
        value_{value} {}

  auto value() const { return value_; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

class ValueExpression : public ConstantExpression {
 private:
  String value_;

 protected:
  explicit ValueExpression(const SyntaxKind kind, const String& value)
      : ConstantExpression{kind},
        value_{value} {}

 public:
  auto& value() const { return value_; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

class NumberExpression final : public ValueExpression {
 public:
  explicit NumberExpression(const String& value)
      : ValueExpression{SyntaxKind::ExprNumber, value} {}
};

class StringExpression final : public ValueExpression {
 public:
  explicit StringExpression(const String& value)
      : ValueExpression{SyntaxKind::ExprString, value} {}
};

class IdentifierExpression final : public ExpressionNode {
  String identifier_;

 public:
  explicit IdentifierExpression(const String& identifier)
      : ExpressionNode{SyntaxKind::ExprIdentifier},
        identifier_{identifier} {}

  auto& identifier() const { return identifier_; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

class CallExpression final : public ExpressionNode {
  String identifier_;

  std::vector<std::shared_ptr<ExpressionNode>> parameters_;

 public:
  explicit CallExpression(const String& identifier)
      : ExpressionNode{SyntaxKind::ExprCall},
        identifier_{identifier} {}

  auto& identifier() const { return identifier_; }

  auto& parameters() const { return parameters_; }

  auto push_parameter(const std::shared_ptr<ExpressionNode>& parameter) -> void {
    parameters_.emplace_back(parameter);
  }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/*
 * Operation Expressions
 */

class Operation : public ExpressionNode {
 public:
  using Expression = std::shared_ptr<ExpressionNode>;

  Operator op_;

 public:
  constexpr Operation(SyntaxKind kind, Operator op)
      : ExpressionNode{kind},
        op_{op} {}

  auto op() const { return op_; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

class UnaryExpression final : public Operation {
  Expression expr_;

 public:
  constexpr explicit UnaryExpression(Operator op)
      : Operation{SyntaxKind::ExprUnary, op} {}

  auto set_expr(const Expression& expr) -> void { expr_ = expr; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

class BinaryExpression final : public Operation {
 private:
  Expression lhs_;
  Expression rhs_;

 public:
  constexpr explicit BinaryExpression(Operator op)
      : Operation{SyntaxKind::ExprBinary, op} {}

  auto& lhs() const { return lhs_; }
  auto& rhs() const { return rhs_; }

  auto set_lhs(const Expression& lhs) -> void { lhs_ = lhs; }

  auto set_rhs(const Expression& rhs) -> void { rhs_ = rhs; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/*
 * Statements
 */

class Statement : public SyntaxNode {
 protected:
  constexpr explicit Statement(SyntaxKind kind)
      : SyntaxNode{kind} {}
};

class StatementBlock final : public SyntaxNode {
 public:
  using Statement           = std::shared_ptr<Statement>;
  using StatementCollection = std::vector<Statement>;

 private:
  StatementCollection statements_;

 public:
  explicit StatementBlock()
      : SyntaxNode(SyntaxKind::Block) {}

  auto& statements() const { return statements_; }

  auto push_statement(const Statement& statement) -> void { statements_.emplace_back(statement); }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

class ExprStatement final : public Statement {
  std::shared_ptr<ExpressionNode> expr_;

 public:
  constexpr ExprStatement()
      : Statement{SyntaxKind::StmtExpr} {}

  auto& expr() const { return expr_; }

  auto set_expr(const std::shared_ptr<ExpressionNode>& expr) { expr_ = expr; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

class ReturnStatement final : public Statement {
  std::shared_ptr<ExpressionNode> expr_;

 public:
  constexpr ReturnStatement()
      : Statement{SyntaxKind::StmtRet} {}

  auto& expr() const { return expr_; }

  auto set_expr(const std::shared_ptr<ExpressionNode>& expr) { expr_ = expr; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/*
 * Branch Statements
 */

class BodyStatement : public Statement {
  std::shared_ptr<StatementBlock> block_;

 protected:
  constexpr explicit BodyStatement(SyntaxKind kind)
      : Statement{kind} {}

 public:
  auto& body() const { return block_; }

  auto set_body(const std::shared_ptr<StatementBlock>& block) { block_ = block; }
};

class IfStatement : public BodyStatement {
  std::shared_ptr<ExpressionNode> expr_;

 protected:
  constexpr explicit IfStatement(SyntaxKind kind)
      : BodyStatement{kind} {}

 public:
  constexpr IfStatement()
      : BodyStatement{SyntaxKind::StmtIf} {}

  auto& expr() const { return expr_; }

  auto set_expr(const std::shared_ptr<ExpressionNode>& expr) -> void { expr_ = expr; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

class ElifStatement final : public IfStatement {
 public:
  constexpr ElifStatement()
      : IfStatement{SyntaxKind::StmtElif} {}
};

class ElseStatement final : public BodyStatement {
 public:
  constexpr ElseStatement()
      : BodyStatement{SyntaxKind::StmtElse} {}
};

class LoopStatement final : public BodyStatement {
 public:
  constexpr LoopStatement()
      : BodyStatement{SyntaxKind::StmtLoop} {}
};

class WhileStatement final : public BodyStatement {
  std::shared_ptr<ExpressionNode> expr_;

 public:
  constexpr WhileStatement()
      : BodyStatement{SyntaxKind::StmtWhile} {}

  auto& expr() const { return expr_; }

  auto set_expr(const std::shared_ptr<ExpressionNode>& expr) -> void { expr_ = expr; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

class ForStatement final : public BodyStatement {
  using Prefix    = std::shared_ptr<Statement>;
  using Condition = std::shared_ptr<ExpressionNode>;
  using Postfix   = std::shared_ptr<ExpressionNode>;

  Prefix prefix_;
  Condition cond_;
  Postfix postfix_;

 public:
  constexpr ForStatement()
      : BodyStatement{SyntaxKind::StmtFor} {}

  auto& prefix() const { return prefix_; }
  auto& cond() const { return cond_; }
  auto& postfix() const { return postfix_; }

  auto set_prefix(const Prefix& prefix) { prefix_ = prefix; }
  auto set_cond(const Condition& cond) { cond_ = cond; }
  auto set_postfix(const Postfix& postfix) { postfix_ = postfix; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

class ForeachStatement final : public BodyStatement {};

/*
 * Definition Statements
 */

class Definition;

class DefStatement final : public Statement {
  std::shared_ptr<Definition> def_;

 public:
  constexpr DefStatement()
      : Statement{SyntaxKind::StmtDef} {}

  auto& def() const { return def_; }

  auto set_def(const std::shared_ptr<Definition>& def) { def_ = def; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/*
 * Type Definitions
 */

class Definition : public SyntaxNode {
  AccessModifier modifier_;
  String name_;

 protected:
  explicit Definition(SyntaxKind kind, const String& name = nullptr)
      : SyntaxNode(kind),
        modifier_{AccessModifier::Public},
        name_{name} {}

 public:
  auto& name() const { return name_; }

  auto set_name(const String& name) noexcept -> void { name_ = name; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

class VarDefinition : public Definition {
 public:
  using Assignment = std::shared_ptr<ExpressionNode>;

 private:
  String type_name_;
  Assignment assignment_;

 public:
  explicit VarDefinition(const String& name       = nullptr,
                         const String& type_name  = nullptr,
                         const Assignment& assign = nullptr)
      : Definition(SyntaxKind::DefVar, name),
        type_name_{type_name},
        assignment_{assign} {}

 public:
  auto& type_name() const { return type_name_; }
  auto& assignment() const { return assignment_; }

  auto set_type_name(const String& type_name) noexcept -> void { type_name_ = type_name; }

  auto set_assignment(const Assignment& assignment) noexcept -> void { assignment_ = assignment; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

class FuncParameter final : public Definition {
 private:
  String type_name_;

 public:
  explicit FuncParameter(const String& name = nullptr)
      : Definition(SyntaxKind::DefParam, name) {}

  auto& type_name() const { return type_name_; }

  auto set_type_name(const String& type_name) noexcept -> void { type_name_ = type_name; }
};

class FuncDefinition final : public Definition {
 public:
  using Parameter           = std::shared_ptr<FuncParameter>;
  using ParameterCollection = std::vector<Parameter>;
  using Body                = std::shared_ptr<StatementBlock>;

 private:
  ParameterCollection parameters_;
  String return_;
  Body body_;

 public:
  explicit FuncDefinition()
      : Definition(SyntaxKind::DefFunc) {}

  auto& parameters() const { return parameters_; }
  auto& return_type() const { return return_; }
  auto& body() const { return body_; }

  void set_return_type(const String& ret_type) { return_ = ret_type; }

  void push_parameter(const Parameter& param) { parameters_.emplace_back(param); }

  auto set_body(const Body& body) { body_ = body; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

class StructDefinition final : public Definition {
  std::vector<std::shared_ptr<Definition>> defs_;

 public:
  explicit StructDefinition()
      : Definition(SyntaxKind::DefStruct) {}

  auto push_definition(const std::shared_ptr<Definition>& def) { defs_.emplace_back(def); }

  auto& definitions() const { return defs_; }
};

/*
 * Import
 */

class Namespace final : public SyntaxNode {
  std::vector<String> namespaces_;

 public:
  explicit Namespace()
      : SyntaxNode{SyntaxKind::Namespace} {}

  NODISCARD auto& namespaces() const { return namespaces_; }

  auto push_namespace(const String& ns) { namespaces_.emplace_back(ns); }

  static const std::shared_ptr<Namespace> root;

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/*
 * Import
 */

class Import final : public SyntaxNode {
  std::vector<String> namespaces_;

 public:
  explicit Import()
      : SyntaxNode{SyntaxKind::Import} {}

  auto& namespaces() const { return namespaces_; }

  auto push_namespace(const String& ns) { namespaces_.emplace_back(ns); }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/*
 * Root
 */

class SyntaxTree final : public SyntaxNode {
 public:
  using Node      = std::shared_ptr<SyntaxNode>;
  using NodeArray = std::vector<Node>;

 private:
  std::shared_ptr<SourceContext> source_;
  NodeArray nodes_;

 public:
  explicit SyntaxTree(const std::shared_ptr<SourceContext>& source)
      : SyntaxNode(SyntaxKind::Source),
        source_{source} {}

  auto& source() const { return source_; }
  auto& nodes() const { return nodes_; }

  auto push_node(const Node& node) { nodes_.emplace_back(node); }

  auto get_namespace() -> std::shared_ptr<Namespace>;

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

using SyntaxTreeCollection = std::vector<std::shared_ptr<SyntaxTree>>;

/*
 * Concepts
 */

template <typename T>
concept IsSyntaxNode = requires(T) { std::is_base_of_v<SyntaxNode, T>; };

template <typename T>
concept IsDefinition = requires(T) { std::is_base_of_v<Definition, T>; };

template <typename T>
concept IsStatement = requires(T) { std::is_base_of_v<Statement, T>; };

template <typename T>
concept IsBodyStatement = requires(T) { std::is_base_of_v<BodyStatement, T>; };