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
// function must be called after readChar return '\n'
std::string getExpression();
void markError(size_t pos);