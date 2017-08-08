#pragma once

#include "transformImpl/TransformImpl.h"
#include <type_traits> // std::make_unsigned

// Done in template way for future 32 and 16 bits calculating
// and not only for cpp types

template <typename T>
std::string toOctal(T val, bool leadingZeros = false)
{
  using UnsignedT = typename std::make_unsigned<T>::type;
  size_t totalAmountOfDigits = (sizeof(T) * 8 / 3) + (sizeof(T) * 8 % 3 != 0);
  if (val == 0)
  {
    size_t amountZeros = leadingZeros ? totalAmountOfDigits  : 1;
    return std::string(amountZeros, '0');
  }
  std::string result;
  HexCalcImpl::toBaseImpl<8>(result, static_cast<UnsignedT>(val));
  return leadingZeros ?
         std::string(totalAmountOfDigits - result.size(), '0') + result :
         result;
}

template <typename T>
std::string toHex(T val, bool leadingZeros = false)
{
  using UnsignedT = typename std::make_unsigned<T>::type;
  if (val == 0)
  {
    size_t amountZeros = leadingZeros ? sizeof(T) * 2 : 1;
    return std::string(amountZeros, '0');
  }
  std::string result;
  HexCalcImpl::toHexImpl(result, static_cast<UnsignedT>(val));
  return leadingZeros ?
         std::string(sizeof(T)*2 - result.size(), '0') + result :
         result;
}

template <typename T>
std::string toChars(T val, bool leadingZeros = false)
{
  using UnsignedT = typename std::make_unsigned<T>::type;
  if (val == 0)
  {
    size_t amountZeros = leadingZeros ? sizeof(T) : 0;
    return std::string(amountZeros, '\00');
  }
  std::string result;
  HexCalcImpl::toCharsImpl(result, static_cast<UnsignedT>(val));
  return leadingZeros ?
         std::string(sizeof(T) - result.size(), '\00') + result :
         result;
}

template <typename T>
std::string toBinary(T val, bool leadingZeros = false)
{
  using UnsignedT = typename std::make_unsigned<T>::type;
  if (val == 0)
  {
    size_t amount = leadingZeros ? sizeof(T) * 8 : 1;
    return std::string(amount, '0');
  }
  std::string result;
  HexCalcImpl::toBaseImpl<2>(result, static_cast<UnsignedT>(val));
  return leadingZeros ?
         std::string(sizeof(T)*8 - result.size(), '0') + result :
         result;
}