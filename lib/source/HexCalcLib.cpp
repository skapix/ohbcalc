#include "hexCalc.h"
#include "HexCalcImplementation.h"
#include <algorithm>

#include <gsl/gsl>
#include <gsl/string_span>

using namespace std;

// TODO: watch c++ precedence

//static_assert(is_sorted(std::begin(binaryOperations), std::end(binaryOperations)), "Operations are not sorted");


HexCalc::HexCalc()
  : pImpl(new HexCalcImplementation())
{}

HexCalc::~HexCalc() {}

int64_t HexCalc::eval(const std::string &expression)
{
  return pImpl->eval(expression);
}