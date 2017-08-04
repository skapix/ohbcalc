#include "TokenOperation.h"
#include "ohbException.h"
#include <algorithm>
#include <cmath>

using namespace std;

// straightforward implementation
int64_t expodential(int64_t a, int64_t b)
{
  while (b > 0)
  {
    a *= 10;
    --b;
  }
  while (b < 0)
  {
    a /= 10;
    ++b;
    if (a == 0)
      return 0;
  }
  return a;
}

int64_t checkForZero(int64_t val)
{
  if (!val)
  {
    throw OperationException("Division by zero");
  }
  return val;
}

// Operations with different Associativity should have different precedence!
const BinaryOperation binaryOperations[]
  {
    BinaryOperation(Associativity::Left, 15, "<<", [](int64_t a, int64_t b){ return a << b;}),
    BinaryOperation(Associativity::Left, 15, ">>", [](int64_t a, int64_t b){ return a >> b;}),
    BinaryOperation(Associativity::Right, 40, "**", [](int64_t a, int64_t b){ return std::pow(a, b);}),
    BinaryOperation(Associativity::Left, 20, "+", [](int64_t a, int64_t b){ return a + b;}),
    BinaryOperation(Associativity::Left, 20, "-", [](int64_t a, int64_t b){ return a - b;}),
    BinaryOperation(Associativity::Left, 30, "*", [](int64_t a, int64_t b){ return a * b;}),
    BinaryOperation(Associativity::Left, 30, "/", [](int64_t a, int64_t b){ return a / checkForZero(b);}),
    BinaryOperation(Associativity::Left, 30, "%", [](int64_t a, int64_t b){ return a % checkForZero(b);}),
    BinaryOperation(Associativity::Left, 8, "||", [](int64_t a, int64_t b){ return a || b;}),
    BinaryOperation(Associativity::Left, 8, "|", [](int64_t a, int64_t b){ return a | b;}),
    BinaryOperation(Associativity::Left, 9, "^", [](int64_t a, int64_t b){ return a ^ b;}),
    BinaryOperation(Associativity::Left, 10, "&&", [](int64_t a, int64_t b){ return a && b;}),
    BinaryOperation(Associativity::Left, 10, "&", [](int64_t a, int64_t b){ return a & b;}),
    BinaryOperation(Associativity::Left, 12, "<=", [](int64_t a, int64_t b){ return a <= b;}),
    BinaryOperation(Associativity::Left, 12, "<", [](int64_t a, int64_t b){ return a < b;}),
    BinaryOperation(Associativity::Left, 12, ">=", [](int64_t a, int64_t b){ return a >= b;}),
    BinaryOperation(Associativity::Left, 12, ">", [](int64_t a, int64_t b){ return a > b;}),
    BinaryOperation(Associativity::Left, 11, "==", [](int64_t a, int64_t b){ return a == b;}),
    BinaryOperation(Associativity::Left, 11, "!=", [](int64_t a, int64_t b){ return a != b;})
  };

bool operator==(const BinaryOperation &left, const CStringView str)
{
  return left.getRepresentation() ==  str;
}


template <typename T>
const T* getOperation(const T* begin, const T* end, CStringView str, size_t &endOperator)
{
  if (str.length() == 0)
  {
    return nullptr;
  }

  // TODO: binary operation could be found more efficiently
  for (const T *it = begin; it != end; ++it)
  {
    CStringView operation = it->getRepresentation();
    if (str.length() < operation.length())
    {
      continue;
    }

    if (operation == str.subspan(0, operation.length()))
    {
      endOperator += operation.length();
      return it;
    }
  }
  return nullptr;
}

const BinaryOperation *getBinaryOperation(const CStringView str, size_t &endOperator)
{
  endOperator = getFirstNonEmptySymbol(str);
  return getOperation(begin(binaryOperations), end(binaryOperations), str.subspan(endOperator), endOperator);
}

const UnaryOperation unaryOperations[]
{
  UnaryOperation("-", [](int64_t val){ return -val;}),
  UnaryOperation("~", [](int64_t val){ return ~val;}),
  UnaryOperation("!", [](int64_t val){ return static_cast<int64_t >(!val);})
};

bool operator==(const UnaryOperation &left, const CStringView str)
{
  return left.getRepresentation() ==  str;
}

const UnaryOperation *getUnaryOperation(const CStringView str, size_t &endOperator)
{
  endOperator = getFirstNonEmptySymbol(str);
  return getOperation(begin(unaryOperations), end(unaryOperations), str.subspan(endOperator), endOperator);
}
