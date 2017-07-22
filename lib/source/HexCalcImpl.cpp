#include "HexCalcImpl.h"
#include "CommonDefines.h"
#include "TokenOperation.h"

using namespace std;

namespace
{

bool notTokenChar(const char c)
{
  return isspace(c) || !isalnum(c);
}

bool validToken(CStringView expression)
{
  const char lastChar = tolower(getLastChar(expression));
  if (expression.length() == 1 && isalpha(lastChar))
  {
    return false;
  }
  for (size_t i = 0; i < expression.length() -1; ++i)
  {
    if (!isdigit(expression[i]))
      return false;
  }

  return isdigit(lastChar) || lastChar == 'h' || lastChar == 'i';
}

int64_t tokenFromBinary(CStringView token)
{
  int64_t result = 0;
  for (auto item : token)
  {
    assert((item == 0 || item == 1) && "token should be a binary digit");
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
    assert(isdigit(item) && "token should be a digit");
    result *= 10;
    result += item - '0';
  }
  return result;
}

int64_t tokenToInt(CStringView token)
{
  char lastLetter = tolower(getLastChar(token));
  if (lastLetter == 'h')
    return tokenFromHex(token.subspan(0, token.length()-1));
  if (lastLetter == 'i')
    return tokenFromBinary(token.subspan(0, token.length()-1));
  else
    return tokenFromDecimal(token);
}

int64_t getToken(CStringView expression, size_t &pos)
{
  size_t startToken = getFirstNonEmptySymbol(expression);
  size_t endToken = find_if(expression.begin() + startToken,
                            expression.end(), notTokenChar) - expression.begin();
  pos = endToken;
  if (startToken == endToken)
  {
    throw std::logic_error("Can't get token");
  }

  CStringView token = expression.subspan(startToken, endToken -  startToken);
  if (!validToken(token))
  {
    throw std::logic_error(string("Token ") + std::string(token.begin(), token.end()) + " is not valid");
  }

  return tokenToInt(token);

}

inline const TokenOperation * getOperationAndCheck(
  CStringView &expression, size_t &pos)
{
  const TokenOperation * op = getOperation(expression, pos);
  if (op == nullptr && pos != expression.length())
  {
    throw logic_error("Operation wasn't met");
  }
  expression = expression.subspan(pos);
  return op;
}

int64_t getExpression(const int64_t arg1, const TokenOperation *operation, CStringView expression)
{
  size_t pos;
  const int64_t arg2 = getToken(expression, pos);
  expression = expression.subspan(pos);
  auto nextOperation = getOperationAndCheck(expression, pos);
  if (nextOperation == nullptr)
  {
    // we reached end of expression
    return operation->apply(arg1, arg2);
  }
  if (operation->getPrecedence() > nextOperation->getPrecedence() ||
      nextOperation->getAssociativity() == Associativity::Left)
  {
    int64_t result = operation->apply(arg1, arg2);
    return getExpression(result, nextOperation, expression);
    //return
  }
  int64_t result = getExpression(arg2, nextOperation, expression);
  return operation->apply(arg1, result);

}

int64_t getExpression(CStringView expression)
{
  size_t pos;
  int64_t arg1 = getToken(expression, pos);
  expression = expression.subspan(pos);

  auto op = getOperationAndCheck(expression, pos);
  if (op == nullptr)
  {
    // we reached end of expression
    return arg1;
  }

  return getExpression(arg1, op, expression);

}

void toBinaryImpl(string& result, uint64_t val)
{
  if (val > 1)
  {
    toBinaryImpl(result, val / 2);
    result.push_back('0' + val % 2);
  }
  else if (val == 1)
    result.push_back('1');
}

string toBinary(uint64_t val)
{
  if (val == 0)
    return "0";
  string result;
  toBinaryImpl(result, val);
  return result;
}

char intToHex(const uint64_t hexDigit)
{
  assert(hexDigit < 16);
  return hexDigit < 10 ? '0' + hexDigit : 'A' + hexDigit - 10;
}
void toHexImpl(string& result, uint64_t val)
{
  if (val >= 16)
  {
    toHexImpl(result, val / 16);
    result.push_back(intToHex(val % 16));
  }
  else if (val < 16)
    result.push_back(intToHex(val));
}

string toHex(uint64_t val)
{
  if (val == 0)
    return "0";
  string result;
  toHexImpl(result, val);
  return result;
}

void toCharsImpl(string &result, uint64_t val)
{
  if (val >= 256)
  {
    toCharsImpl(result, val / 256);
    result.push_back(static_cast<char>(val % 256));
  }
  else
  {
    result.push_back(static_cast<char>(val));
  }
}

string toChars(uint64_t val)
{
  if (val == 0)
    return "";
  string result;
  toCharsImpl(result, val);
  return result;
}

} // namespace


uint64_t HexCalcImpl::getUint64() const {
  return static_cast<uint64_t>(result);
}

int64_t HexCalcImpl::getInt64() const {
  return result;
}

string HexCalcImpl::getBinary() const {
  return toBinary(static_cast<uint64_t>(result));
}

string HexCalcImpl::getHex() const {
  return toHex(static_cast<uint64_t>(result));
}

string HexCalcImpl::getChars() const {
  return toChars(static_cast<uint64_t>(result));
}

void HexCalcImpl::eval(const std::string &expression) {
  result = getExpression(expression);
}
