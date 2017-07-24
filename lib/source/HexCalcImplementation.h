#pragma once

#include "CommonDefines.h"
#include <string>


class HexCalcImplementation {
public:
  HexCalcImplementation() = default;
  ~HexCalcImplementation() = default;
  int64_t eval(const std::string &expression);

private:
};
