#pragma once

#include "transformImpl/TransformImpl.h"
#include <type_traits> // std::make_unsigned

// Done in template way for future 32 and 16 bits calculating
// and not only for cpp types
// uint64_t
// TODO: toOctal

template <typename T>
std::string toHex(T val, bool leadingZeroes = false)
{
  using UnsignedT = typename std::make_unsigned<T>::type;
  if (val == 0)
    return "0";
  std::string result;
  HexCalcImpl::toHexImpl(result, static_cast<UnsignedT>(val));
  return leadingZeroes ?
         std::string(sizeof(T)*2 - result.size(), '0') + result :
         result;
}

template <typename T>
std::string toChars(T val, bool leadingZeroes = false)
{
  using UnsignedT = typename std::make_unsigned<T>::type;
  if (val == 0)
    return "";
  std::string result;
  HexCalcImpl::toCharsImpl(result, static_cast<UnsignedT>(val));
  return leadingZeroes ?
         std::string(sizeof(T) - result.size(), '0') + result :
         result;
}

template <typename T>
std::string toBinary(T val, bool leadingZeroes = false)
{
  using UnsignedT = typename std::make_unsigned<T>::type;
  if (val == 0)
    return "0";
  std::string result;
  HexCalcImpl::toBinaryImpl(result, static_cast<UnsignedT>(val));
  return leadingZeroes ?
         std::string(sizeof(T)*8 - result.size(), '0') + result :
         result;
}