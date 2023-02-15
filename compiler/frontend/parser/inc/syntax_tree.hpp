// Copyright (c) 2023 Jacob R. Green
// All Rights Reserved.

#pragma once

#ifndef __cplusplus
#error
#endif  //__cplusplus

#include <string>
#include <memory>
#include <utility>
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

enum class Modifiers : uint8_t {
};

enum class AccessModifier : uint8_t {
  Private,
  Module,
  Internal,
  Protected,
  Public
};

auto to_string(AccessModifier modifier) -> std::string_view;

/**
 * BaseSyntax
 * \todo implement file position of syntax
 */
class BaseSyntax
#ifdef TRACE
    : public xml::Serializable
#endif
{
 public:
  using Pointer = std::unique_ptr<BaseSyntax>;

 private:
  const SyntaxKind kind_;
  const FilePosition pos_;

 protected:
  explicit constexpr BaseSyntax(const SyntaxKind kind, const FilePosition& pos)
      : kind_{kind},
        pos_{pos} {}

 public:
  virtual ~BaseSyntax() = default;

  NODISCARD constexpr auto kind() const noexcept { return kind_; }
  NODISCARD constexpr auto& pos() const noexcept { return pos_; }

 protected:
#ifdef TRACE
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/**
 * BaseExpression
 */
class BaseExpression : public BaseSyntax {
 public:
  using Pointer = std::unique_ptr<BaseExpression>;

 protected:
  explicit constexpr BaseExpression(const SyntaxKind kind, const FilePosition& pos)
      : BaseSyntax{kind, pos} {}
};

/**
 * ConstantExpression
 * \brief Base class for all constants
 */
class BaseConstantExpression : public BaseExpression {
 public:
  using Pointer = std::unique_ptr<BaseConstantExpression>;

 protected:
  constexpr explicit BaseConstantExpression(SyntaxKind kind, const FilePosition& pos)
      : BaseExpression{kind, pos} {}
};

/**
 * BooleanExpression
 */
class BooleanExpression final : public BaseConstantExpression {
 public:
  using Pointer = std::unique_ptr<BooleanExpression>;

 private:
  bool value_;

 public:
  constexpr explicit BooleanExpression(const FilePosition& pos, bool value)
      : BaseConstantExpression{SyntaxKind::ExprBool, pos},
        value_{value} {}

  NODISCARD auto value() const { return value_; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/**
 * BaseConstantValueExpression
 * \brief Abstract base class for constants represented as strings
 */
class BaseConstantValueExpression : public BaseConstantExpression {
 public:
  using Pointer = std::unique_ptr<BaseConstantValueExpression>;

 private:
  std::string value_;

 protected:
  explicit BaseConstantValueExpression(const SyntaxKind kind,
                                       const FilePosition& pos,
                                       std::string value)
      : BaseConstantExpression{kind, pos},
        value_{std::move(value)} {}

 public:
  NODISCARD auto& value() const { return value_; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/**
 * NumberExpression
 */
class NumberExpression final : public BaseConstantValueExpression {
 public:
  using Pointer = std::unique_ptr<NumberExpression>;

  explicit NumberExpression(const FilePosition& pos, const std::string& value)
      : BaseConstantValueExpression{SyntaxKind::ExprNumber, pos, value} {}
};

/**
 * StringExpression
 */
class StringExpression final : public BaseConstantValueExpression {
 public:
  using Pointer = std::unique_ptr<StringExpression>;

  explicit StringExpression(const FilePosition& pos, const std::string& value)
      : BaseConstantValueExpression{SyntaxKind::ExprString, pos, value} {}
};

/**
 * IdentifierExpression
 */
class IdentifierExpression final : public BaseExpression {
 public:
  using Pointer = std::unique_ptr<IdentifierExpression>;

 private:
  std::string identifier_;

 public:
  explicit IdentifierExpression(const FilePosition& pos, std::string identifier)
      : BaseExpression{SyntaxKind::ExprIdentifier, pos},
        identifier_{std::move(identifier)} {}

  NODISCARD auto& identifier() const { return identifier_; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/**
 * CallExpression
 */
class CallExpression final : public BaseExpression {
 public:
  using Pointer = std::unique_ptr<CallExpression>;

 private:
  std::string identifier_;

  std::vector<BaseExpression::Pointer> parameters_;

 public:
  explicit CallExpression(const FilePosition& pos, std::string identifier)
      : BaseExpression{SyntaxKind::ExprCall, pos},
        identifier_{std::move(identifier)} {}

  NODISCARD auto& identifier() const { return identifier_; }
  NODISCARD auto& parameters() const { return parameters_; }

  auto push_parameter(BaseExpression::Pointer parameter) -> void {
    parameters_.emplace_back(std::move(parameter));
  }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/*
 * Operation Expressions
 */

/**
 * BaseOperation
 * \brief Abstract base class for all operator expressions
 */
class BaseOperation : public BaseExpression {
 public:
  using Pointer = std::unique_ptr<BaseOperation>;

 private:
  Operator op_;

 protected:
  constexpr BaseOperation(SyntaxKind kind, const FilePosition& pos, Operator op)
      : BaseExpression{kind, pos},
        op_{op} {}

 public:
  NODISCARD auto op() const { return op_; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/**
 * UnaryExpression
 */
class UnaryExpression final : public BaseOperation {
 public:
  using Pointer = std::unique_ptr<UnaryExpression>;

 private:
  BaseExpression::Pointer expr_;

 public:
  constexpr explicit UnaryExpression(const FilePosition& pos, Operator op)
      : BaseOperation{SyntaxKind::ExprUnary, pos, op} {}

  auto set_expr(BaseExpression::Pointer expr) -> void { expr_ = std::move(expr); }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/**
 * BinaryExpression
 */
class BinaryExpression final : public BaseOperation {
 public:
  using Pointer = std::unique_ptr<BinaryExpression>;

 private:
  BaseExpression::Pointer lhs_;
  BaseExpression::Pointer rhs_;

 public:
  constexpr explicit BinaryExpression(const FilePosition& pos, Operator op)
      : BaseOperation{SyntaxKind::ExprBinary, pos, op} {}

  NODISCARD auto& lhs() const { return lhs_; }
  NODISCARD auto& rhs() const { return rhs_; }

  auto set_lhs(BaseExpression::Pointer lhs) -> void { lhs_ = std::move(lhs); }

  auto set_rhs(BaseExpression::Pointer rhs) -> void { rhs_ = std::move(rhs); }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/*
 * Statements
 */

/**
 * BaseStatement
 * \brief Base class for all
 */
class BaseStatement : public BaseSyntax {
 public:
  using Pointer = std::unique_ptr<BaseStatement>;

 protected:
  constexpr explicit BaseStatement(SyntaxKind kind, const FilePosition& pos)
      : BaseSyntax{kind, pos} {}
};

/**
 * StatementBlock
 */
class StatementBlock final : public BaseSyntax {
 public:
  using Pointer = std::unique_ptr<StatementBlock>;

 private:
  using Statement           = BaseStatement::Pointer;
  using StatementCollection = std::vector<Statement>;

  StatementCollection statements_;

 public:
  explicit StatementBlock(const FilePosition& pos)
      : BaseSyntax(SyntaxKind::Block, pos) {}

  NODISCARD auto& statements() const { return statements_; }

  auto push_statement(Statement statement) -> void {
    statements_.emplace_back(std::move(statement));
  }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/**
 * ExpressionStatement
 */
class ExpressionStatement final : public BaseStatement {
 public:
  using Pointer = std::unique_ptr<ExpressionStatement>;

 private:
  BaseExpression::Pointer expr_;

 public:
  constexpr ExpressionStatement(const FilePosition& pos)
      : BaseStatement{SyntaxKind::StmtExpr, pos} {}

  NODISCARD auto& expr() const { return expr_; }

  auto set_expr(BaseExpression::Pointer expr) { expr_ = std::move(expr); }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/**
 * ReturnStatement
 */
class ReturnStatement final : public BaseStatement {
 public:
  using Pointer = std::unique_ptr<ReturnStatement>;

 private:
  BaseExpression::Pointer expr_;

 public:
  constexpr ReturnStatement(const FilePosition& pos)
      : BaseStatement{SyntaxKind::StmtRet, pos} {}

  NODISCARD auto& expr() const { return expr_; }

  auto set_expr(BaseExpression::Pointer expr) { expr_ = std::move(expr); }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/*
 * Branch Statements
 */

/**
 * BodyStatement
 * \brief Abstract base class for all statements that contain an embedded statement block
 */
class BaseBodyStatement : public BaseStatement {
 public:
  using Pointer = std::unique_ptr<BaseBodyStatement>;

 private:
  StatementBlock::Pointer block_;

 protected:
  constexpr explicit BaseBodyStatement(SyntaxKind kind, const FilePosition& pos)
      : BaseStatement{kind, pos} {}

 public:
  NODISCARD auto& body() const { return block_; }

  auto set_body(StatementBlock::Pointer block) { block_ = std::move(block); }
};

/**
 * IfStatement
 */
class IfStatement : public BaseBodyStatement {
 public:
  using Pointer = std::unique_ptr<IfStatement>;

 private:
  BaseExpression::Pointer expr_;

 protected:
  constexpr explicit IfStatement(SyntaxKind kind, const FilePosition& pos)
      : BaseBodyStatement{kind, pos} {}

 public:
  constexpr IfStatement(const FilePosition& pos)
      : BaseBodyStatement{SyntaxKind::StmtIf, pos} {}

  NODISCARD auto& expr() const { return expr_; }

  auto set_expr(BaseExpression::Pointer expr) -> void { expr_ = std::move(expr); }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/**
 * ElifStatement
 */
class ElifStatement final : public IfStatement {
 public:
  using Pointer = std::unique_ptr<ElifStatement>;

  constexpr ElifStatement(const FilePosition& pos)
      : IfStatement{SyntaxKind::StmtElif, pos} {}
};

/**
 * ElseStatement
 */
class ElseStatement final : public BaseBodyStatement {
 public:
  using Pointer = std::unique_ptr<ElseStatement>;

  constexpr ElseStatement(const FilePosition& pos)
      : BaseBodyStatement{SyntaxKind::StmtElse, pos} {}
};

/**
 * LoopStatement
 */
class LoopStatement final : public BaseBodyStatement {
 public:
  using Pointer = std::unique_ptr<LoopStatement>;

  constexpr LoopStatement(const FilePosition& pos)
      : BaseBodyStatement{SyntaxKind::StmtLoop, pos} {}
};

/**
 * WhileStatement
 */
class WhileStatement final : public BaseBodyStatement {
 public:
  using Pointer = std::unique_ptr<WhileStatement>;

 private:
  std::unique_ptr<BaseExpression> expr_;

 public:
  constexpr WhileStatement(const FilePosition& pos)
      : BaseBodyStatement{SyntaxKind::StmtWhile, pos} {}

  NODISCARD auto& expr() const { return expr_; }

  auto set_expr(std::unique_ptr<BaseExpression> expr) -> void { expr_ = std::move(expr); }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/**
 * ForStatement
 */
class ForStatement final : public BaseBodyStatement {
 public:
  using Pointer = std::unique_ptr<ForStatement>;

 private:
  using Prefix    = BaseStatement::Pointer;
  using Condition = BaseExpression::Pointer;
  using Postfix   = BaseExpression::Pointer;

  Prefix prefix_;
  Condition cond_;
  Postfix postfix_;

 public:
  constexpr ForStatement(const FilePosition& pos)
      : BaseBodyStatement{SyntaxKind::StmtFor, pos} {}

  NODISCARD auto& prefix() const { return prefix_; }
  NODISCARD auto& cond() const { return cond_; }
  NODISCARD auto& postfix() const { return postfix_; }

  auto set_prefix(Prefix prefix) { prefix_ = std::move(prefix); }
  auto set_cond(Condition cond) { cond_ = std::move(cond); }
  auto set_postfix(Postfix postfix) { postfix_ = std::move(postfix); }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/**
 * ForeachStatement
 */
class ForeachStatement final : public BaseBodyStatement {};

/*
 * Type Definitions
 */

/**
 * NamespaceDeclaration
 */
class NamespaceDeclaration final : public BaseSyntax {
 public:
  using Pointer = std::unique_ptr<NamespaceDeclaration>;

 private:
  std::vector<std::string> namespaces_;

 public:
  explicit NamespaceDeclaration(const FilePosition& pos)
      : BaseSyntax{SyntaxKind::Namespace, pos} {}

  NODISCARD auto& namespaces() const { return namespaces_; }

  auto push_namespace(const std::string& ns) { namespaces_.emplace_back(ns); }

  static const std::unique_ptr<NamespaceDeclaration> root;

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/**
 * NamespaceImport
 */
class NamespaceImport final : public BaseSyntax {
 public:
  using Pointer = std::unique_ptr<NamespaceImport>;

 private:
  std::vector<std::string> namespaces_;

 public:
  explicit NamespaceImport(const FilePosition& pos)
      : BaseSyntax{SyntaxKind::Import, pos} {}

  NODISCARD auto& namespaces() const { return namespaces_; }

  auto push_namespace(const std::string& ns) { namespaces_.emplace_back(ns); }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/**
 * BaseDefinition
 * \brief Abstract base class for all type definitions
 */
class BaseDefinition : public BaseSyntax {
 public:
  using Pointer = std::unique_ptr<BaseDefinition>;

 private:
  AccessModifier modifier_;
  std::string name_;

 protected:
  explicit BaseDefinition(SyntaxKind kind, const FilePosition& pos)
      : BaseSyntax{kind, pos},
        modifier_{AccessModifier::Public} {}

  explicit BaseDefinition(SyntaxKind kind, const FilePosition& pos, std::string name)
      : BaseSyntax{kind, pos},
        modifier_{AccessModifier::Public},
        name_{std::move(name)} {}

 public:
  NODISCARD auto& name() const { return name_; }

  auto set_name(const std::string& name) noexcept -> void { name_ = name; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/**
 * DefinitionStatement
 */
class DefinitionStatement final : public BaseStatement {
 public:
  using Pointer = std::unique_ptr<DefinitionStatement>;

 private:
  std::unique_ptr<BaseDefinition> def_;

 public:
  constexpr DefinitionStatement(const FilePosition& pos)
      : BaseStatement{SyntaxKind::StmtDef, pos} {}

  NODISCARD auto& def() const { return def_; }

  auto set_def(std::unique_ptr<BaseDefinition> def) { def_ = std::move(def); }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/**
 * VariableDefinition
 */
class VariableDefinition final : public BaseDefinition {
 public:
  using Pointer = std::unique_ptr<VariableDefinition>;

 private:
  using Assignment = std::unique_ptr<BaseExpression>;

  std::string type_name_;
  Assignment assignment_;
  bool mutable_ = false;

 public:
  explicit VariableDefinition(const FilePosition& pos)
      : BaseDefinition{SyntaxKind::DefVar, pos} {}

 public:
  NODISCARD auto& type_name() const { return type_name_; }
  NODISCARD auto& assignment() const { return assignment_; }

  NODISCARD auto is_typed() const { return !type_name_.empty(); }
  NODISCARD auto is_assigned() const { return assignment_ != nullptr; }
  NODISCARD auto is_mutable() const { return mutable_; }

  auto set_type_name(const std::string& type_name) noexcept -> void { type_name_ = type_name; }

  auto set_assignment(Assignment assignment) noexcept -> void {
    assignment_ = std::move(assignment);
  }

  auto set_mutable(bool mut) noexcept -> void { mutable_ = mut; }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

/**
 * FunctionParameter
 */
class FunctionParameter final : public BaseDefinition {
 public:
  using Pointer = std::unique_ptr<FunctionParameter>;

 private:
  std::string type_name_;

 public:
  explicit FunctionParameter(const FilePosition& pos, const std::string& name)
      : BaseDefinition{SyntaxKind::DefParam, pos, name} {}

  NODISCARD auto& type_name() const { return type_name_; }

  NODISCARD auto is_type_auto() const { return type_name_.empty(); }

  auto set_type_name(const std::string& type_name) noexcept -> void { type_name_ = type_name; }
};

/**
 * FunctionDefinition
 */
class FunctionDefinition final : public BaseDefinition {
 public:
  using Pointer = std::unique_ptr<FunctionDefinition>;

 private:
  using Parameter           = FunctionParameter::Pointer;
  using ParameterCollection = std::vector<Parameter>;
  using Body                = StatementBlock::Pointer;

 private:
  ParameterCollection parameters_;
  std::string return_;
  Body body_;

 public:
  explicit FunctionDefinition(const FilePosition& pos)
      : BaseDefinition{SyntaxKind::DefFunc, pos} {}

  NODISCARD auto& parameters() const { return parameters_; }
  NODISCARD auto& return_type() const { return return_; }
  NODISCARD auto& body() const { return body_; }

  NODISCARD auto is_return_auto() const { return return_.empty(); }

  void set_return_type(const std::string& ret_type) { return_ = ret_type; }

  void push_parameter(Parameter param) { parameters_.emplace_back(std::move(param)); }

  auto set_body(Body body) { body_ = std::move(body); }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

class StructDefinition;
class ObjectDefinition;

/**
 * BaseStructureDefinition
 */
class BaseStructureDefinition : public BaseDefinition {
 public:
  using Pointer = std::unique_ptr<BaseStructureDefinition>;

 private:
  std::vector<VariableDefinition::Pointer> variables_;
  std::vector<FunctionDefinition::Pointer> functions_;
  std::vector<std::unique_ptr<StructDefinition>> structs_;
  std::vector<std::unique_ptr<ObjectDefinition>> objects_;

 protected:
  explicit BaseStructureDefinition(SyntaxKind kind, const FilePosition& pos)
      : BaseDefinition(kind, pos) {}

 public:
  NODISCARD auto& variables() const { return variables_; }
  NODISCARD auto& functions() const { return functions_; }
  NODISCARD auto& structs() const { return structs_; }
  NODISCARD auto& objects() const { return objects_; }

  constexpr auto& push_var(VariableDefinition::Pointer var) {
    return variables_.emplace_back(std::move(var));
  }

  constexpr auto& push_func(FunctionDefinition::Pointer var) {
    return functions_.emplace_back(std::move(var));
  }

  constexpr auto& push_struct(std::unique_ptr<StructDefinition> str) {
    return structs_.emplace_back(std::move(str));
  }

  constexpr auto& push_object(std::unique_ptr<ObjectDefinition> str) {
    return objects_.emplace_back(std::move(str));
  }
};

/**
 * StructDefinition
 */
class StructDefinition final : public BaseStructureDefinition {
 public:
  using Pointer = std::unique_ptr<StructDefinition>;

  explicit StructDefinition(const FilePosition& pos)
      : BaseStructureDefinition(SyntaxKind::DefStruct, pos) {}
};

/**
 * ObjectDefinition
 */
class ObjectDefinition final : public BaseStructureDefinition {
 public:
  using Pointer = std::unique_ptr<ObjectDefinition>;

  explicit ObjectDefinition(const FilePosition& pos)
      : BaseStructureDefinition(SyntaxKind::DefObject, pos) {}
};

/**
 * SyntaxTree
 * \brief Root node
 */
class SyntaxTree final : public BaseStructureDefinition {
 public:
  using Pointer = std::unique_ptr<SyntaxTree>;

 private:
  SourceContext::Pointer source_;

  std::vector<NamespaceImport::Pointer> imports_;
  std::vector<NamespaceDeclaration::Pointer> namespaces_;

 public:
  explicit SyntaxTree(SourceContext::Pointer source)
      : BaseStructureDefinition{SyntaxKind::Source, {0, 0}},
        source_{std::move(source)} {}

  NODISCARD auto& source() const { return source_; }

  NODISCARD auto& imports() const { return imports_; }
  NODISCARD auto& namespaces() const { return namespaces_; }

  constexpr auto& push_import(NamespaceImport::Pointer ns) {
    return imports_.emplace_back(std::move(ns));
  }

  constexpr auto& push_namespace(NamespaceDeclaration::Pointer ns) {
    return namespaces_.emplace_back(std::move(ns));
  }

#ifdef TRACE
 protected:
  auto on_serialize(xml::SerializationContext& context) const -> void override;
#endif
};

using SyntaxTreeCollection = std::vector<std::unique_ptr<SyntaxTree>>;

/*
 * Concepts
 */

template <typename T>
concept IsSyntaxNode = requires(T) { std::is_base_of_v<BaseSyntax, T>; };

template <typename T>
concept IsDefinition = requires(T) { std::is_base_of_v<BaseDefinition, T>; };

template <typename T>
concept IsStatement = requires(T) { std::is_base_of_v<BaseStatement, T>; };

template <typename T>
concept IsBodyStatement = requires(T) { std::is_base_of_v<BaseBodyStatement, T>; };