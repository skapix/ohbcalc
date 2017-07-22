#include "TokenOperation.h"
#include <iterator>
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

// Binary operations
const TokenOperation operations[]
  {
    TokenOperation(Associativity::Left, 15, "<<", [](int64_t a, int64_t b){ return a << b;}),
    TokenOperation(Associativity::Left, 15, ">>", [](int64_t a, int64_t b){ return a >> b;}),
    TokenOperation(Associativity::Right, 40, "**", [](int64_t a, int64_t b){ return std::pow(a, b);}),
    TokenOperation(Associativity::Left, 20, "+", [](int64_t a, int64_t b){ return a + b;}),
    TokenOperation(Associativity::Left, 20, "-", [](int64_t a, int64_t b){ return a - b;}),
    TokenOperation(Associativity::Left, 30, "*", [](int64_t a, int64_t b){ return a * b;}),
    TokenOperation(Associativity::Left, 30, "/", [](int64_t a, int64_t b){ return a / b;}),
    TokenOperation(Associativity::Left, 30, "%", [](int64_t a, int64_t b){ return a % b;}),
    TokenOperation(Associativity::Left, 8, "|", [](int64_t a, int64_t b){ return a | b;}),
    TokenOperation(Associativity::Left, 9, "^", [](int64_t a, int64_t b){ return a ^ b;}),
    TokenOperation(Associativity::Left, 10, "&", [](int64_t a, int64_t b){ return a & b;}),
  };


static bool notOperatorChar(const char c)
{
  return isspace(c) || isalnum(c);
}




bool operator<(const TokenOperation &left, const TokenOperation &right)
{
  return left.getRepresentation() <  right.getRepresentation();
}

bool operator==(const TokenOperation &left, const CStringView str)
{
  return left.getRepresentation() ==  str;
}

//static TokenOperation sortedOperations[end(operations) - begin(operations)];

//ArrayView
constexpr ArrayView<const TokenOperation> getOperations() {
  return ArrayView<const TokenOperation>(&operations[0], end(operations) - begin(operations));
}


static constexpr size_t getMaxTokenLenght()
{
  size_t result = 0;
  for (const TokenOperation &op : operations)
    result = max<size_t>(result, op.getRepresentation().length());
  return result;
}


static const size_t maxTokenLength = getMaxTokenLenght();

const TokenOperation *getOperation(const CStringView str, size_t &endOperator)
{
  size_t startSymbol = getFirstNonEmptySymbol(str);
  if (startSymbol == str.size())
  {
    endOperator = startSymbol;
    return nullptr;
  }

  size_t endSymbol = find_if(str.begin() + startSymbol, str.end(), notOperatorChar) - str.begin();
  endOperator = endSymbol;

  if (startSymbol == endSymbol)
  {
    return nullptr;
  }

  CStringView possibleToken = str.subspan(startSymbol, endSymbol - startSymbol);

  auto it = find(begin(operations), end(operations), possibleToken);

  if (it == end(operations))
  {
    throw logic_error(string("Unknown operator ") + string(possibleToken.begin(), possibleToken.end()));
  }
  return &*it;
}
