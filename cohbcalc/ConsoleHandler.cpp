#include "ConsoleHandler.h"
#include "KeyHandler.h"
#include "ExpressionHistory.h"

#include <iostream>
#include <map>
#include <list>
#include <cassert>
#include <array>
#include <algorithm>
#include <cctype> // isspace

using namespace std;

ExpressionHistory g_expressionHistory;
string g_currentExpression;
string::iterator g_currentPosition = g_currentExpression.begin();


void handleChar(char c)
{
  putchar(c);
  g_currentPosition = g_currentExpression.insert(g_currentPosition, c);
  ++g_currentPosition;

  if (g_currentPosition == g_currentExpression.end())
  {
    return;
  }
  int goBack = 0;
  for (auto it = g_currentPosition; it != g_currentExpression.end(); ++it, ++goBack)
  {
    putchar(*it);
  }
  cursorLeft(goBack);
}

string getExpression()
{
  if (!all_of(g_currentExpression.begin(), g_currentExpression.end(),
              [](char c){
                return std::isspace(c); }))
  {
    g_expressionHistory.push_back(g_currentExpression);
  }
  string result = std::move(g_currentExpression);

  g_currentExpression.clear(); // just in case
  g_currentPosition = g_currentExpression.begin();
  return result;
}

void updateStringView()
{
  handleSpecialKey(SpecialKey::Home);
  bool endHistory = g_expressionHistory.getElement() == nullptr;

  size_t szNew = endHistory ? 0 : g_expressionHistory.getElement()->size();


  while (g_currentExpression.size() > szNew)
  {
    handleSpecialKey(SpecialKey::Delete);
  }
  if (endHistory)
  {
    g_currentPosition = g_currentExpression.end();
    return;
  }
  // else write content of history

  g_currentExpression = *g_expressionHistory.getElement();
  fputs(g_currentExpression.c_str(), stdout);
  g_currentPosition = g_currentExpression.end();
}

const int g_pgCount = 5;

void handleSpecialKey(const SpecialKey key) {
  switch (key) {
    case SpecialKey::Up:
      g_expressionHistory.getElement(-1);
      updateStringView();
      break;
    case SpecialKey::Down:
      g_expressionHistory.getElement(1);
      updateStringView();
      break;
    case SpecialKey::PageUp:
      g_expressionHistory.getElement(-g_pgCount);
      updateStringView();
      break;
    case SpecialKey::PageDown:
      g_expressionHistory.getElement(g_pgCount);
      updateStringView();
      break;
    case SpecialKey::Left:
      if (g_currentPosition != g_currentExpression.begin())
      {
        --g_currentPosition;
        cursorLeft();
      }
      break;
    case SpecialKey::Right:
      if (g_currentPosition != g_currentExpression.end())
      {
        ++g_currentPosition;
        cursorRight();
      }
      break;
    case SpecialKey::CtrlLeft:
      do
      {
        handleSpecialKey(SpecialKey::Left);
      } while(g_currentPosition != g_currentExpression.begin() && isalnum(*g_currentPosition));
      break;
    case SpecialKey::CtrlRight:
      do
      {
        handleSpecialKey(SpecialKey::Right);
      } while(g_currentPosition != g_currentExpression.end() && isalnum(*g_currentPosition));
      break;
    case SpecialKey::Home:
      while  (g_currentPosition != g_currentExpression.begin())
      {
        handleSpecialKey(SpecialKey::Left);
      }
      break;
    case SpecialKey::End:
      while (g_currentPosition != g_currentExpression.end())
      {
        handleSpecialKey(SpecialKey::Right);
      }
      break;
    case SpecialKey::Delete:
      if (g_currentPosition == g_currentExpression.end())
      {
        break;
      }
      handleSpecialKey(SpecialKey::Right);
      // go forward
    case SpecialKey::BackSpace:
      if (g_currentPosition != g_currentExpression.begin())
      {
        backspace();
        g_currentPosition = g_currentExpression.erase(std::prev(g_currentPosition));

        int goBack = 0;
        for (auto it = g_currentPosition; it != g_currentExpression.end(); ++it, ++goBack)
        {
          putchar(*it);
        }
        putchar(' ');
        ++goBack;
        for (int i = 0; i < goBack; ++i)
          cursorLeft();
      }
      break;
    case SpecialKey::EndLine:
      putchar('\n');
      break;
    case SpecialKey::Undefined:
      // do nothing
      break;
  }
}

void markError(size_t pos) {
  for (size_t i = 0; i < pos; ++i)
  {
    putchar(' ');
  }
  putchar('^');
  putchar('\n');
}
