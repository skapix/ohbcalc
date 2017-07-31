#include "HexCalcImplementation.h"
#include "CommonDefines.h"
#include "TokenOperation.h"

using namespace std;

namespace
{

// base <= 10
template <int Base>
int64_t tokenFromBase(CStringView token)
{
  assert(Base <= 10 && "Wrong usage");
  int64_t result = 0;
  for (auto item : token)
  {
    if (item < '0' || item >= '0' + Base)
    {
      throw logic_error(string("Can't decode character ") + to_string(item) +  " in base " + to_string(Base));
    }
    result *= Base;
    result += item - '0';
  }
  return result;
}

template <>
int64_t tokenFromBase<16>(CStringView token)
{
  int64_t result = 0;
  for (auto item : token)
  {
    result *= 16;
    if (isdigit(item))
    {
      result += item - '0';
    }
    else if (tolower(item) >= 'a' && tolower(item) <= 'f')
    {
      result += tolower(item) - 'a' + 10;
    }
    else
    {
      throw logic_error(string("Can't decode character ") + to_string(item) +  " in base 16");
    }
  }
  return result;
}

inline char lastIdentificator(CStringView expression)
{
  return tolower(expression.last(1)[0]);
}

int64_t getValue(CStringView expression, size_t &pos)
{
  auto isHex = [](CStringView &number)
  {
    if (number.length() > 2 && number[0] == '0' && tolower(number[1]) == 'x')
    {
      number = number.subspan(2);
      return true;
    }
    if (lastIdentificator(number) == 'h')
    {
      number = number.subspan(0, number.length() - 1);
      return true;
    }
    return false;
  };

  auto isBin = [](CStringView &number)
  {
    if (number.length() > 1 && (lastIdentificator(number) == 'b' || lastIdentificator(number) == 'i'))
    {
      number = number.subspan(0, number.length() - 1);
      return true;
    }
    return false;
  };

  auto isOct = [](CStringView &number)
  {
    if (number.length() > 1 && number[0] == '0')
    {
      number = number.subspan(1);
      return true;
    }
    if (number.length() > 1 && lastIdentificator(number) == 'o')
    {
      number = number.subspan(0, number.length() - 1);
      return true;
    }
    return false;
  };


  for (pos = 0; pos < static_cast<size_t>(expression.length()) && isalnum(expression[pos]); ++pos)
  {}

  CStringView numberView = expression.subspan(0, pos);

  if (numberView.empty())
  {
    // no data found
    throw std::logic_error("Can't get token");
  }

  if (isHex(numberView))
  {
    return tokenFromBase<16>(numberView);
  }
  if (isBin(numberView))
  {
    return tokenFromBase<2>(numberView);
  }
  if (isOct(numberView))
  {
    return tokenFromBase<8>(numberView);
  }
  return tokenFromBase<10>(numberView);
}

int64_t getExpressionImpl(CStringView expression, size_t &pos);

// pos is out argument
int64_t getToken(CStringView expression, size_t &pos)
{
  size_t localPos;
  if (auto unary = getUnaryOperation(expression, pos))
  {
    int64_t result = getToken(expression.subspan(pos), localPos);
    pos += localPos;
    return unary->apply(result);
  }

  if (expression[pos] == '(')
  {
    size_t localPos;
    int64_t token = getExpressionImpl(expression.subspan(++pos), localPos);
    pos += localPos;
    if (static_cast<size_t>(expression.length()) <= pos || expression[pos] != ')')
    {
      throw logic_error("Unexpected symbol. Expected ')'");
    }
    ++pos;
    return token;
  }


  int64_t result = getValue(expression.subspan(pos), localPos);
  pos += localPos;
  return result;
}


int64_t getExpressionImpl(const int64_t arg1, const BinaryOperation *operation, CStringView expression,
                          size_t &pos)
{
  size_t localPos = 0;
  const int64_t arg2 = getToken(expression, pos);
  expression = expression.subspan(pos);
  auto nextOperation = getBinaryOperation(expression, localPos);
  expression = expression.subspan(localPos);
  pos += localPos;
  if (nextOperation == nullptr)
  {
    // we reached end of expression
    return operation->apply(arg1, arg2);
  }

  const int operationPrecedence = operation->getPrecedence();
  const int nextOperationPrecedence = nextOperation->getPrecedence();
  if (operationPrecedence > nextOperationPrecedence ||
    (operationPrecedence == nextOperationPrecedence &&
      nextOperation->getAssociativity() == Associativity::Left))
  {
    int64_t newArg = operation->apply(arg1, arg2);
    int64_t result = getExpressionImpl(newArg, nextOperation, expression, localPos);
    pos += localPos;
    return result;
  }

  int64_t newArg = getExpressionImpl(arg2, nextOperation, expression, localPos);
  pos += localPos;
  return operation->apply(arg1, newArg);
}

int64_t getExpressionImpl(CStringView expression, size_t &pos)
{
  size_t localPos = 0;
  int64_t arg1 = getToken(expression, pos);
  expression = expression.subspan(pos);

  auto op =  getBinaryOperation(expression, localPos);
  expression = expression.subspan(localPos);
  pos += localPos;
  if (op == nullptr)
  {
    // we reached end of expression
    return arg1;
  }

  int64_t result = getExpressionImpl(arg1, op, expression, localPos);
  pos += localPos;
  return result;
}


} // namespace


int64_t HexCalcImplementation::eval(const std::string &expression) {
  size_t pos;
  int64_t result = getExpressionImpl(expression, pos);
  if (pos != static_cast<size_t>(expression.length()))
  {
    throw logic_error(string("Can't parse all expression, stopped at position ") + std::to_string(pos));
  }
  return result;
}
