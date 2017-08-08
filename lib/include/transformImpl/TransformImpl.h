#pragma once

#include <cassert>
#include <string>

namespace HexCalcImpl
{


inline char digitToHex(const uint8_t hexDigit)
{
  assert(hexDigit < 16);
  return hexDigit < 10 ? '0' + hexDigit : 'A' + hexDigit - 10;
}

template <typename T>
void toHexImpl(std::string& result, T val)
{
  if (val >= 16)
  {
    toHexImpl(result, val / 16);
    result.push_back(digitToHex(static_cast<uint8_t>(val % 16)));
  }
  else if (val < 16)
  {
    result.push_back(digitToHex(static_cast<uint8_t>(val)));
  }
}

template <typename T>
void toCharsImpl(std::string &result, T val)
{
  if (val >= 256)
  {
    toCharsImpl(result, val / 256);
    result.push_back(static_cast<char>(val % 256));
  }
  else
  {
    result.push_back(static_cast<char>(val));
  }
}

template <int Base, typename T>
void toBaseImpl(std::string& result, T val)
{
  if (val >= Base)
  {
    toBaseImpl<Base, T>(result, val / Base);
    result.push_back(static_cast<char>('0' + val % Base));
  }
  else
  {
    result.push_back('0' + val);
  }
}

}