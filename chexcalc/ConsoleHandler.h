#pragma once

#include <variant>
#include <string>


enum class SpecialKey : char
{
  Undefined,
  Left,
  Right,
  Up,
  Down,
  PageUp,
  PageDown,
  CtrlLeft,
  CtrlRight,
  Home,
  End,
  Delete,
  BackSpace,
  EndLine
};

using Character = std::variant<char, SpecialKey>;


Character readChar();

void initConsole();
void deinitConsole();

void handleSpecialKey(const SpecialKey key);
void handleChar(char c);
std::string getExpression();