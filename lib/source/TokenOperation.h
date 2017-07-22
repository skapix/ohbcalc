#pragma once

#include "CommonDefines.h"
#include <functional>

enum class Associativity
{
  Left,
  Right
};

class TokenOperation
{
public:
  TokenOperation(Associativity associativity, const int precedence, const CStringView representation,
  std::function<int64_t(int64_t, int64_t)> implementation)
    : associativity(associativity)
    , precedence(precedence)
    , representation(representation)
    , implementation(implementation)
  {}

  constexpr Associativity getAssociativity() const { return associativity;}
  constexpr int getPrecedence() const { return precedence; }
  constexpr CStringView getRepresentation() const { return representation; }
  int64_t apply(int64_t a, int64_t b) const { return implementation(a,b);}
private:
  Associativity associativity;
  int precedence;
  CStringView representation;
  std::function<int64_t(int64_t, int64_t)> implementation;
};

constexpr ArrayView<const TokenOperation> getOperations();

const TokenOperation *getOperation(const CStringView str, size_t &endOperator);