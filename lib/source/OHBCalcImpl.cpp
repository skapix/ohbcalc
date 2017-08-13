#include "OHBCalcImpl.h"
#include "ohbException.h"
#include "CommonDefines.h"
#include "TokenOperation.h"

using namespace std;

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

template <>
int64_t tokenFromBase<16>(CStringView token, size_t &pos)
{
  pos = 0;
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
      throw OHBException(pos, string("Can't decode character ") + std::string(1, item) +  " in base 16");
    }
    ++pos;
  }
  return result;
}

inline char lastIdentificator(CStringView expression)
{
  return tolower(expression.last(1)[0]);
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

  size_t idenSz = 0;
  for (; idenSz < static_cast<size_t>(expression.length()) && isalnum(expression[idenSz]); ++idenSz)
  {}

  CStringView numberView = expression.subspan(0, idenSz);

  if (numberView.empty())
  {
    // no data found
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



int64_t getExpressionImpl(const int64_t arg1, const BinaryOperation &operation, CStringView expression,
                          size_t &pos)
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


  const int64_t arg2 = getToken(expression, pos);
  expression = expression.subspan(pos);


  auto nextOperation = getBinaryOperation(expression, localPos);
  expression = expression.subspan(localPos);

  if (nextOperation == nullptr)
  {
    // we reached the end of expression
    return binaryOpWrapper(operation, arg2);
  }
  pos += localPos;

  const int operationPrecedence = operation.getPrecedence();
  const int nextOperationPrecedence = nextOperation->getPrecedence();
  if (operationPrecedence > nextOperationPrecedence ||
    (operationPrecedence == nextOperationPrecedence &&
      nextOperation->getAssociativity() == Associativity::Left))
  {
    int64_t newArg = binaryOpWrapper(operation, arg2);
    int64_t result = getExpressionImpl(newArg, *nextOperation, expression, localPos);
    return result;
  }
  savedPos = pos;

  int64_t newArg = getExpressionImpl(arg2, *nextOperation, expression, localPos);
  return binaryOpWrapper(operation, newArg);
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

  return getExpressionImpl(arg1, *op, expression, localPos);
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
