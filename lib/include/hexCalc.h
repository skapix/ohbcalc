#pragma once

#include <memory>

class HexCalcImpl;

class HexCalc
{
public:
  HexCalc();
  ~HexCalc();
  void eval(const std::string &expression);
  uint64_t getUint64() const;
  int64_t getInt64() const;
  std::string getBinary() const;
  std::string getHex() const;
  std::string getChars() const;

private:
  std::unique_ptr<HexCalcImpl> pImpl;
};