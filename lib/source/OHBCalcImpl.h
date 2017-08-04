#pragma once

#include "CommonDefines.h"
#include <string>


class OHBCalcImpl {
public:
  OHBCalcImpl() = default;
  ~OHBCalcImpl() = default;
  int64_t eval(const std::string &expression);

private:
};
