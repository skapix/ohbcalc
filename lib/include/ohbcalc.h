#pragma once

#include <memory>
#include <string>

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