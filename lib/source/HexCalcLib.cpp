#include "hexCalc.h"
#include <string>

using namespace std;

// Decided not to use hard-weighted libraries like
struct StringView
{

};


enum class Associativity
{
  Left,
  Right
};

// TODO: add ~

class TokenOperation
{
public:
  TokenOperation(Associativity associativity, const int precedence, const string &representation)
    : associativity(associativity)
    , precedence(precedence)
    , representation(representation)
  {}

  Associativity getAssociativity() const { return associativity;}
  int getPrecedence() const { return precedence; }
  const string &getRepresentation() const { return representation; }

private:
  Associativity associativity;
  int precedence;
  const string representation;
};

// TODO: watch c++ precedence
// TODO: add unary operators ~, -

// Binary operations
const TokenOperation operations[]
  {
    TokenOperation(Associativity::Left, 20, "+"),
    TokenOperation(Associativity::Left, 20, "-"),
    TokenOperation(Associativity::Left, 30, "*"),
    TokenOperation(Associativity::Left, 30, "/"),
    TokenOperation(Associativity::Left, 30, "%"),
    TokenOperation(Associativity::Left, 8, "|"),
    TokenOperation(Associativity::Left, 9, "^"),
    TokenOperation(Associativity::Left, 10, "&"),
    TokenOperation(Associativity::Left, 15, "<<"),
    TokenOperation(Associativity::Left, 15, ">>"),
    TokenOperation(Associativity::Right, 99, "e"),
    TokenOperation(Associativity::Right, 40, "**"),
  };


