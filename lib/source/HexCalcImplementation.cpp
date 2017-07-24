#include "HexCalcImplementation.h"
#include "CommonDefines.h"
#include "TokenOperation.h"

using namespace std;

namespace
{

int64_t tokenFromBinary(CStringView token)
{
  int64_t result = 0;
  for (auto item : token)
  {
    if (item != '0' && item != '1')
    {
      throw logic_error("token should be a binary digit");
    }
    result *= 2;
    result += item - '0';
  }
  return result;
}

int64_t tokenFromHex(CStringView token)
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
      assert(false && "Met not a hex number");
    }
  }
  return result;
}

int64_t tokenFromDecimal(CStringView token)
{
  int64_t result = 0;
  for (auto item : token)
  {
    if (!isdigit(item))
    {
      throw logic_error("token should be a digit");
    }
    result *= 10;
    result += item - '0';
  }
  return result;
}

bool isHexDigit(char c)
{
  return isdigit(c) || (tolower(c) >= 'a' && tolower(c) <= 'f');
}

int64_t getExpressionImpl(CStringView expression, size_t &pos);

// pos is out argument
int64_t getToken(CStringView expression, size_t &pos)
{
  if (auto unary = getUnaryOperation(expression, pos))
  {
    size_t localPos;
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


  // start parsing number
  size_t startNum = pos;
  // find first non-digit
  for (;pos < static_cast<size_t>(expression.length()) && isHexDigit(expression[pos]); ++pos)
  {}

  if (startNum == pos)
  {
    // no digits found
    throw std::logic_error("Can't get token");
  }
  if (pos == static_cast<size_t>(expression.length()))
  {
    return tokenFromDecimal(expression.subspan(startNum));
  }

  if (tolower(expression[pos]) == 'i')
  {
    return tokenFromBinary(expression.subspan(startNum, pos++));
  }
  if (tolower(expression[pos]) == 'h')
  {
    return tokenFromHex(expression.subspan(startNum, pos++));
  }
  return tokenFromDecimal(expression.subspan(startNum, pos - startNum));
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
    throw logic_error("Can't parse all expression"); // TODO: change
  }
  return result;
}
