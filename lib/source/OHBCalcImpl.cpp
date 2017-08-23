#include "OHBCalcImpl.h"
#include "ohbException.h"
#include "CommonDefines.h"
#include "TokenOperation.h"
#include <map>

using namespace std;

// Important structure for moving position if the exception was thrown
struct PositionMover
{
  PositionMover(size_t &posToMove, const size_t &posToAdd)
  : m_disable(false)
   , m_dest(posToMove)
  , m_src(posToAdd)
  {}
  void disable() {m_disable = true;}
  ~PositionMover()
  {
    if (!m_disable)
    {
      m_dest += m_src;
    }
  }

private:
  bool m_disable;
  size_t &m_dest;
  const size_t &m_src;
};

namespace
{

// base <= 10
template <int Base>
int64_t tokenFromBase(CStringView token, size_t &pos)
{
  assert(Base <= 10 && "Wrong usage");
  pos = 0;
  int64_t result = 0;
  for (auto item : token)
  {
    if (item < '0' || item >= '0' + Base)
    {
      throw logic_error(string("Can't decode character ") + std::string(1, item) +  " in base " + to_string(Base));
    }
    result *= Base;
    result += item - '0';
    ++pos;
  }
  return result;
}

uint8_t charToHex(char c)
{
  if (isdigit(c))
  {
    return static_cast<uint8_t>(c - '0');
  }

  char lc = static_cast<char>(tolower(c));
  if (lc >= 'a' && lc <= 'f')
  {
    return static_cast<uint8_t>(lc - 'a' + 10);
  }
  throw logic_error("Can't decode character " + string(1, c) +  " in base 16");
}

template <>
int64_t tokenFromBase<16>(CStringView token, size_t &pos)
{
  pos = 0;
  int64_t result = 0;
  for (auto item : token)
  {
    result *= 16;
    result += charToHex(item);
    ++pos;
  }
  return result;
}

inline bool isHexDigit(char c)
{
  return (c >= '0' && c <= '9') || (tolower(c) >= 'a' && tolower(c) <= 'f');
}

static const std::map<char, uint8_t> g_specials = {{'t', '\t'}, {'n', '\n'}, {'r', '\r'}, {'n', '\n'}, {'\\', '\\'}, {'"', '"'}};

uint8_t getSpecialCharacter(CStringView specialStart, size_t &pos)
{
  if (specialStart.empty())
  {
    --pos;
    throw logic_error("Expected special character");
  }

  if (isHexDigit(specialStart[0]))
  {
    if (specialStart.size() < 2)
    {
      --pos;
      throw logic_error("Special character should contain 2 hex digits");
    }
    if (!isHexDigit(specialStart[1]))
    {
      ++pos;
      throw logic_error("Non-hex special character");
    }
    int result = charToHex(specialStart[0]) * 16 + charToHex(specialStart[1]);
    assert(numeric_limits<uint8_t>::max() >= result);
    pos += 2;
    return static_cast<uint8_t>(result);
  }
  // handle special symbol
  auto it = g_specials.find(specialStart[0]);
  if (it == g_specials.end())
  {
    throw  logic_error("Unknown special character");
  }
  ++pos;
  return it->second;
}

template <>
int64_t tokenFromBase<256>(CStringView token, size_t &pos)
{
  pos = 0;
  int64_t result = 0;
  assert(token.length() >= 0 && "Wrong logic");
  const size_t sz = static_cast<size_t>(token.length());
  for (pos = 0; pos < sz;)
  {
    result *= 256;
    char item = token[pos];
    if (item != '\\')
    {
      result += static_cast<uint8_t>(item);
      ++pos;
      continue;
    }
    ++pos;
    result += getSpecialCharacter(token.subspan(pos), pos);
  }
  return result;
}

inline char lastIdentificator(CStringView expression)
{
  return static_cast<char>(tolower(expression.last(1)[0]));
}

int64_t getValue(CStringView expression, size_t &pos)
{
  pos = 0;
  size_t localPos = 0;
  auto mover = PositionMover(pos, localPos);

  auto isHex = [&localPos](CStringView &number)
  {
    if (number.length() > 2 && number[0] == '0' && tolower(number[1]) == 'x')
    {
      localPos = 2;
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

  auto isBin = [&localPos](CStringView &number)
  {
    if (number.length() > 1 && (lastIdentificator(number) == 'b' || lastIdentificator(number) == 'i'))
    {
      number = number.subspan(0, number.length() - 1);
      return true;
    }
    return false;
  };

  auto isOct = [&localPos](CStringView &number)
  {
    if (number.length() > 1 && number[0] == '0')
    {
      localPos = 1;
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

  auto isChar = [&localPos](CStringView &number)
  {

    if (number.length() >= 1 && number[0] == '"') // >= for exception
    {
      int itPos = 1;
      for (; itPos < number.length(); ++itPos)
      {
        if (number[itPos] == '"' && number[itPos-1] != '\\')
          break;
      }
      if (itPos == number.length())
      {
        localPos = itPos;
        throw logic_error("Expected end of character expression");
      }
      localPos = 1;
      number = number.subspan(1, itPos - 1);
      return true;
    }
    return false;
  };

  size_t idenSz = 0;
  for (; idenSz < static_cast<size_t>(expression.length()) && isalnum(expression[idenSz]); ++idenSz)
  {}

  CStringView numberView = expression.subspan(0, idenSz);

  if (numberView.empty())
  {
    // no usual data found
    numberView = expression;
    if (isChar(numberView))
    {
      int64_t result = tokenFromBase<256>(numberView, pos);
      localPos *= 2; // counting first and last identificator
      return result;
    }
    throw std::logic_error("Can't get identificator");
  }

  if (isHex(numberView))
  {
    int64_t result = tokenFromBase<16>(numberView, pos);
    // add suffix position. Note that every number in special base has
    // a suffix if it has no prefix
    pos += localPos == 0;
    return result;
  }
  if (isBin(numberView))
  {
    int64_t result = tokenFromBase<2>(numberView, pos);
    pos += localPos == 0;
    return result;
  }
  if (isOct(numberView))
  {
    int64_t result = tokenFromBase<8>(numberView, pos);
    pos += localPos == 0;
    return result;
  }

  if (!isdigit(lastIdentificator(numberView)))
  {
    pos += numberView.size() - 1;
    throw logic_error(string("Unknown suffix ") + string(1, lastIdentificator(numberView)));
  }
  return tokenFromBase<10>(numberView, pos);
}

int64_t getExpressionImpl(CStringView expression, size_t &pos);

// pos is out argument
int64_t getToken(CStringView expression, size_t &pos)
{
  pos = 0;
  size_t localPos = 0;
  auto mover = PositionMover(pos, localPos);

  if (auto unary = getUnaryOperation(expression, pos))
  {
    int64_t result = getToken(expression.subspan(pos), localPos);
    return unary->apply(result);
  }
  if (static_cast<size_t>(expression.length()) == pos)
  {
    throw logic_error("Unexpected end of statement");
  }

  if (expression[pos] == '(')
  {
    int64_t token = getExpressionImpl(expression.subspan(++pos), localPos);
    pos += localPos;
    mover.disable();
    if (static_cast<size_t>(expression.length()) <= pos || expression[pos] != ')')
    {
      throw logic_error("Unexpected symbol. Expected ')'");
    }
    ++pos;
    return token;
  }


  // can throw
  int64_t result = getValue(expression.subspan(pos), localPos);
  return result;
}


std::pair<int64_t, const BinaryOperation*> getExpressionImpl(const int64_t arg1, const BinaryOperation &operation, CStringView expression,
                          size_t &pos, const BinaryOperation *prevOperation = nullptr)
{
  pos = 0;
  size_t localPos = 0;
  auto mover = PositionMover(pos, localPos);
  size_t savedPos = static_cast<size_t>(-operation.getRepresentation().size());

  auto binaryOpWrapper = [&](const BinaryOperation &op, int64_t arg2) -> int64_t
  {
    try
    {
      return op.apply(arg1, arg2);
    }
    catch (const OperationException &operationEx)
    {
      mover.disable();
      pos = savedPos;
      throw std::logic_error(operationEx.what());
    }
  };

  auto checkPrev = [&](const BinaryOperation *nextOperation) -> bool
  {
    if (prevOperation)
    {
      const int prevOpPrecedence = prevOperation->getPrecedence();
      const int nextOpPrecedence = nextOperation->getPrecedence();
      if (prevOpPrecedence > nextOpPrecedence ||
          (prevOpPrecedence == nextOpPrecedence &&
           prevOperation->getAssociativity() == Associativity::Left))
      {
        mover.disable();
        return true;
      }
    }
    return false;
  };


  const int64_t arg2 = getToken(expression, pos);
  expression = expression.subspan(pos);


  auto nextOperation = getBinaryOperation(expression, localPos);
  expression = expression.subspan(localPos);

  if (nextOperation == nullptr)
  {
    // we reached the end of expression
    return {binaryOpWrapper(operation, arg2), nullptr};
  }
  pos += localPos;


  if (operation.getPrecedence() > nextOperation->getPrecedence() ||
    (operation.getPrecedence() == nextOperation->getPrecedence() &&
      nextOperation->getAssociativity() == Associativity::Left))
  {
    int64_t newArg = binaryOpWrapper(operation, arg2);
    if (checkPrev(nextOperation))
    {
      return {newArg, nextOperation};
    }

    return getExpressionImpl(newArg, *nextOperation, expression, localPos, prevOperation);
  }
  savedPos = pos;


  while (true)
  {
    auto localResult = getExpressionImpl(arg2, *nextOperation, expression, localPos, &operation);
    int64_t result = binaryOpWrapper(operation, localResult.first);
    expression = expression.subspan(localPos);

    if (localResult.second == nullptr)
    {
      return {result, nullptr};
    }
    pos += localPos;
    if (checkPrev(localResult.second))
    {
      return {result, localResult.second};
    }
    return getExpressionImpl(result, *localResult.second, expression, localPos, prevOperation);
  }
}

int64_t getExpressionImpl(CStringView expression, size_t &pos)
{
  pos = 0;
  size_t localPos = 0;
  auto mover = PositionMover(pos, localPos);
  int64_t arg1 = getToken(expression, localPos);
  pos += localPos;
  expression = expression.subspan(localPos);

  auto op =  getBinaryOperation(expression, localPos);
  expression = expression.subspan(localPos);
  pos += localPos;
  if (op == nullptr)
  {
    // we reached end of expression
    mover.disable();
    return arg1;
  }

  return getExpressionImpl(arg1, *op, expression, localPos).first;

}


} // namespace


int64_t OHBCalcImpl::eval(const std::string &expression) {
  size_t pos = 0;
  int64_t result;
  try
  {
    result = getExpressionImpl(expression, pos);
  }
  catch (const exception& exc)
  {
    // rethrow
    throw OHBException(pos, exc.what());
  }
  if (pos != static_cast<size_t>(expression.length()))
  {
    throw OHBException(pos, "Can't parse all expression");
  }
  return result;
}
