#pragma once

#include <memory>

class HCException;

class HexCalcImplementation;

class HexCalc
{
public:
  HexCalc();
  ~HexCalc();
  int64_t eval(const std::string &expression);
private:
  std::unique_ptr<HexCalcImplementation> pImpl;
};