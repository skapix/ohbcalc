#include "ohbcalc.h"
#include "OHBCalcImpl.h"

using namespace std;

OHBCalc::OHBCalc()
  : pImpl(new OHBCalcImpl())
{}

OHBCalc::~OHBCalc() {}

int64_t OHBCalc::eval(const std::string &expression)
{
  return pImpl->eval(expression);
}