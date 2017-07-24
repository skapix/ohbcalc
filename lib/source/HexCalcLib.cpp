#include "hexCalc.h"
#include "HexCalcImplementation.h"

using namespace std;

HexCalc::HexCalc()
  : pImpl(new HexCalcImplementation())
{}

HexCalc::~HexCalc() {}

int64_t HexCalc::eval(const std::string &expression)
{
  return pImpl->eval(expression);
}