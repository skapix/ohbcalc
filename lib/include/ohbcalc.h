#pragma once

#include <memory>

class OHBCalcImpl;

class OHBCalc
{
public:
  OHBCalc();
  ~OHBCalc();
  int64_t eval(const std::string &expression);
private:
  std::unique_ptr<OHBCalcImpl> pImpl;
};