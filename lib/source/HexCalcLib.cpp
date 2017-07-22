#include "hexCalc.h"
#include "HexCalcImpl.h"
#include <algorithm>

#include <gsl/gsl>
#include <gsl/string_span>

using namespace std;



// TODO: watch c++ precedence
// TODO: add unary operators ~, -


//static_assert(is_sorted(std::begin(operations), std::end(operations)), "Operations are not sorted");


HexCalc::HexCalc()
  : pImpl(new HexCalcImpl())
{}

HexCalc::~HexCalc() {}

void HexCalc::eval(const std::string &expression)
{
  pImpl->eval(expression);
}

uint64_t HexCalc::getUint64() const
{
  return pImpl->getUint64();
}

int64_t HexCalc::getInt64() const
{
  return pImpl->getInt64();
}
std::string HexCalc::getBinary() const
{
  return pImpl->getBinary();
}
std::string HexCalc::getHex() const
{
  return pImpl->getHex();
}
std::string HexCalc::getChars() const
{
  return pImpl->getChars();
}
