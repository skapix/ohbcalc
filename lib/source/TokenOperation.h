#pragma once

#include "CommonDefines.h"
#include <functional>
#include <string>

class UnaryOperation {
public:
  UnaryOperation(CStringView representation, std::function<int64_t(int64_t)> implementation) noexcept
    : representation(representation)
    , implementation(std::move(implementation))
  {}

  constexpr CStringView getRepresentation() const { return representation; }

  int64_t apply(int64_t val) const { return implementation(val); }

private:
  CStringView representation;
  std::function<int64_t(int64_t)> implementation;
};

enum class Associativity
{
  Left,
  Right
};

class BinaryOperation {
public:
  BinaryOperation(Associativity associativity, const int precedence, const CStringView representation,
                  std::function<int64_t(int64_t, int64_t)> implementation) noexcept
    : associativity(associativity)
    , precedence(precedence)
    , representation(representation)
    , implementation(std::move(implementation))
  {}

  constexpr Associativity getAssociativity() const { return associativity; }
  constexpr int getPrecedence() const { return precedence; }
  constexpr CStringView getRepresentation() const { return representation; }
  int64_t apply(int64_t a, int64_t b) const { return implementation(a, b); }

private:
  Associativity associativity;
  int precedence;
  CStringView representation;
  std::function<int64_t(int64_t, int64_t)> implementation;
};

const BinaryOperation *getBinaryOperation(const CStringView str, size_t &endOperator);
const UnaryOperation *getUnaryOperation(const CStringView str, size_t &endOperator);

class OperationException : public std::exception {
public:
  OperationException(const std::string &error)
    : m_error(error)
  {}

  virtual const char *what() const noexcept override { return m_error.c_str(); }

private:
  std::string m_error;
};
