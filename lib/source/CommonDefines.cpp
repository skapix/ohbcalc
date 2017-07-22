#include "CommonDefines.h"

size_t getFirstNonEmptySymbol(CStringView str)
{
  auto it = std::find_if(str.begin(), str.end(), [](const char c){return !std::isspace(c);});
  return it - str.begin();
}

char getLastChar(CStringView str)
{
  assert(str.length() && "Unexpected to be empty");
  return str[str.length() -1];
}
