#pragma once

#include <string>


class HexCalcImpl {
public:
  HexCalcImpl() = default;
  ~HexCalcImpl() = default;
  void eval(const std::string &expression);
  uint64_t getUint64() const;
  int64_t getInt64() const;
  std::string getBinary() const;
  std::string getHex() const;
  std::string getChars() const;

private:
  int64_t result;
};
