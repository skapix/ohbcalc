#include "ohbcalc.h"
#include "KeyHandler.h"
#include "ohbTransform.h"
#include "ohbException.h"
#include "ConsoleHandler.h"
#include "OutputRepresentation.h"

#include <iostream>
#include <set>
#include <iomanip>

using namespace std;

namespace
{

struct InitConsole
{
  InitConsole() { initConsole(); }

  ~InitConsole() { deinitConsole(); }
};

bool isExitSequence(const string &str)
{
  const set<string> exitSequence = {"q", "quit", "exit", "Exit"};
  return exitSequence.find(str) != exitSequence.end();
}

const string g_inputExpression = "> ";

void inputExpression() { cout << g_inputExpression; }

string getLine()
{
  while (true)
  {
    Character c = readChar();
    if (char *letter = get_if<char>(&c))
    {
      handleChar(*letter);
    }
    else
    {
      SpecialKey key = get<SpecialKey>(c);
      handleSpecialKey(key);
      if (key == SpecialKey::EndLine)
      {
        return getExpression();
      }
    }
  }
}

inline void calculate(OHBCalc &calculator, OutputRepresentation &representation, const string &expression,
                      size_t offset = 0)
{
  try
  {
    auto result = calculator.eval(expression);
    representation.printResult(result);
  }
  catch (const OHBException &expr)
  {
    markError(expr.getPos() + offset);
    cout << expr.what() << endl;
  }
  catch (const exception &expr)
  {
    cout << "Unexpected error: " << expr.what() << endl;
  }
}

} // namespace


int main(int argc, const char *argv[])
{
  OHBCalc calculator;
  OutputRepresentation representation;
  string expression;

  bool shouldExit = false;
  for (int i = 1; i < argc; ++i)
  {
    expression = argv[i];
    if (representation.parseOutputSettings(expression))
    {
      continue;
    }
    cout << expression << endl;
    calculate(calculator, representation, expression, 0);
    shouldExit = true;
  }

  if (shouldExit)
  {
    return 0;
  }

  InitConsole consoleInitializer;
  while (true)
  {
    inputExpression();
    expression = getLine();
    if (isExitSequence(expression))
    {
      break;
    }
    if (representation.parseOutputSettings(expression))
    {
      continue;
    }

    if (expression.empty())
    {
      continue;
    }

    calculate(calculator, representation, expression, g_inputExpression.size());
  }

  return 0;
}