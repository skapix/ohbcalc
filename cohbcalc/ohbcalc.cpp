#include "ohbcalc.h"
#include "ohbTransform.h"
#include "ConsoleHandler.h"

#include <iostream>
#include <set>

using namespace std;

struct InitConsole
{
  InitConsole() { initConsole(); }
  ~InitConsole() { deinitConsole(); }
};

static bool isExitSequence(const string &str)
{
  const set<string> exitSequence = {"q", "quit", "exit", "Exit"};
  return exitSequence.find(str) != exitSequence.end();
}

static void inputExpression()
{
  cout << "> ";
}


// TODO: refactor
static string getLine()
{
  while(true) {
    Character  c = readChar();
    if (char *letter = get_if<char>(&c))
    {
      handleChar(*letter);
    }
    else
    {
      SpecialKey key = get<SpecialKey>(c);
      handleSpecialKey(key);
      if (key == SpecialKey::EndLine) {
        return getExpression();
      }
    }
  }
}

// TODO: add arg: calculate and exit
// TODO: add output settings
// TODO: add history (up, down)



int main(int argc, const char * argv[])
{
  InitConsole consoleInitializer;

  OHBCalc calculator;
  string expression;
  while (true) {
    inputExpression();
    expression = getLine();
    if (isExitSequence(expression))
    {
      break;
    }

    if (expression.empty())
    {
      continue;
    }

    try {
      auto result = calculator.eval(expression);
      cout << "d: " << result << endl;
      cout << "h: " << toHex(result) << endl;
      cout << "i: " << toBinary(result) << endl;
    }
    catch (const exception &expr) // TODO: change exception type; TODO: fix zero exception
    {

    }
  }

  return 0;
}