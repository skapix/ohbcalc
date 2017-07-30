#include "ConsoleHandler.h"
#include "KeyBindings.h"

#include <iostream>
#include <map>
#include <list>
#include <cassert>
#include <array>
#include <algorithm>

using namespace std;

// linux only headers
#include <termios.h>
#include <sys/ioctl.h>


int kbhit() {
  int bytesWaiting;
  ioctl(0, FIONREAD, &bytesWaiting);
  return bytesWaiting;
}

static termios oldT, newT;

void initTermios(int echo)
{
  tcgetattr(0, &oldT); /* grab old terminal i/o settings */
  newT = oldT; /* make new settings same as old settings */
  newT.c_lflag &= ~ICANON; /* disable buffered i/o */
  newT.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(0, TCSANOW, &newT); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void)
{
  tcsetattr(0, TCSANOW, &oldT);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo)
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch()
{
  return getch_(0);
}

/* Read 1 character with echo */
char getche()
{
  return getch_(1);
}

///////////////////////////
///////////////////////////
// fast calculating expressions independently from control sequence values
///////////////////////////
///////////////////////////

constexpr const int ch_left[] = {CHKEY_LEFT};
constexpr const int ch_right[] = {CHKEY_RIGHT};
constexpr const int ch_up[] = {CHKEY_UP};
constexpr const int ch_down[] = {CHKEY_DOWN};
constexpr const int ch_ctrlleft[] = {CHKEY_CTRLLEFT};
constexpr const int ch_ctrlright[] = {CHKEY_CTRLRIGHT};
constexpr const int ch_pgup[] = {CHKEY_PAGEUP};
constexpr const int ch_pgdown[] = {CHKEY_PAGEDOWN};
constexpr const int ch_home[] = {CHKEY_HOME};
constexpr const int ch_end[] = {CHKEY_END};
constexpr const int ch_backspace[] = {CHKEY_BACKSPACE};
constexpr const int ch_delete[] = {CHKEY_DELETE};
constexpr const int ch_endline[] = {CHKEY_ENDLINE};

#define EQSIZEB(constv) (sizeof(constv) == AmountElements)
template <int AmountElements>
const int g_sz = EQSIZEB(ch_left) + EQSIZEB(ch_right) + EQSIZEB(ch_up) +
                 EQSIZEB(ch_down) + EQSIZEB(ch_ctrlleft) + EQSIZEB(ch_ctrlright) +
                 EQSIZEB(ch_pgup) + EQSIZEB(ch_pgdown) + EQSIZEB(ch_home) +
                 EQSIZEB(ch_end) + EQSIZEB(ch_backspace) + EQSIZEB(ch_delete) +
                 EQSIZEB(ch_endline);
#define PTRSIZE(constv) make_pair(constv, sizeof(constv) / sizeof(constv[0]))
constexpr const pair<const int*, int> g_allSpecial[] = {PTRSIZE(ch_left) , PTRSIZE(ch_right), PTRSIZE(ch_up),
                                  PTRSIZE(ch_down), PTRSIZE(ch_ctrlleft), PTRSIZE(ch_ctrlright),
                                  PTRSIZE(ch_pgup), PTRSIZE(ch_pgdown), PTRSIZE(ch_home), PTRSIZE(ch_end),
                                  PTRSIZE(ch_backspace), PTRSIZE(ch_delete), PTRSIZE(ch_endline)};


struct CharComparator {
  bool operator()(const std::pair<const int *, int> &left, const std::pair<const int *, int> &right)  const{
    if (left.second < right.second)
      return true;
    if (left.second > right.second)
      return false;
    for (int i = 0; i < left.second; ++i) {
      if (left.first[i] < right.first[i])
        return true;
      if (left.first[i] > right.first[i])
        return false;
    }
    return false;
  }
};

#define MAPVALUE(constv, enumKey) make_pair(PTRSIZE(constv), SpecialKey::enumKey)
const map<pair<const int*, int>, SpecialKey, CharComparator> g_sequence = {MAPVALUE(ch_left, Left),
                                                           MAPVALUE(ch_right, Right), MAPVALUE(ch_up, Up),
                                                           MAPVALUE(ch_down, Down), MAPVALUE(ch_ctrlleft, CtrlLeft),
                                                           MAPVALUE(ch_ctrlright, CtrlRight), MAPVALUE(ch_pgup, PageUp),
                                                           MAPVALUE(ch_pgdown, PageDown), MAPVALUE(ch_home, Home),
                                                           MAPVALUE(ch_end, End), MAPVALUE(ch_backspace, BackSpace),
                                                           MAPVALUE(ch_delete, Delete), MAPVALUE(ch_endline, EndLine)
};

#undef PTRSIZE
#undef MAPVALUE

constexpr int getAmountStartUnique() noexcept
{
  int amountUnique = 0;
  const int sz = sizeof(g_allSpecial)/sizeof(g_allSpecial[0]);
  for (int i = 0; i < sz; ++i)
  {
    int j = 0;
    for (; j < i && g_allSpecial[i].first[0] != g_allSpecial[j].first[0]; ++j);
    if (i == j)
    {
      ++amountUnique;
    }
  }
  return amountUnique;
}

constexpr const auto g_amountStartUnique = getAmountStartUnique();

constexpr array<int, g_amountStartUnique> getSpecialKeyStartSequence() noexcept
{
  array<int, g_amountStartUnique> result = {};
  const int sz = sizeof(g_allSpecial)/sizeof(g_allSpecial[0]);
  int current = 0;
  for (int i = 0; i < sz; ++i)
  {
    int j = 0;
    for (; j < i && g_allSpecial[i].first[0] != g_allSpecial[j].first[0]; ++j);
    if (i == j)
    {
      result[current++] = g_allSpecial[i].first[0];
    }
  }
  return result;
}

constexpr const auto g_specialKeyStartSequence = getSpecialKeyStartSequence();

constexpr const int maxSequenceLength() noexcept
{
  int result = 0;
  for (auto specialKey : g_allSpecial)
  {
    result = max(result, specialKey.second);
  }
  return result;
}

constexpr const int g_maxSequenceLength = maxSequenceLength();
///////////////////////////
///////////////////////////

static bool isSpecialKeyStart(const int c)
{
  return find(g_specialKeyStartSequence.begin(), g_specialKeyStartSequence.end(),  c)
         != g_specialKeyStartSequence.end();
}

Character readChar()
{
  Character c;
  int read[g_maxSequenceLength];
  read[0] = getch();

  if (!isSpecialKeyStart(read[0])) {
    c = read[0];
    return c;
  }
  // parse SpecialKey
  int i = 1;
  for (; i < g_maxSequenceLength; ++i)
  {
    if (kbhit())
    {
      read[i] = getch();
      continue;
    }
    break;
  }
  // skip all unread special keys
  bool notFullSequence = false;
  while(kbhit()) {
    getch();
    notFullSequence = true;
  }
  if (notFullSequence)
  {
    return SpecialKey::Undefined;
  }

//  for (int k = 0; k < i; ++k)
//  {
//    cout << read[k] << endl;
//  }

  auto it = g_sequence.find(make_pair(read, i));
  return it == g_sequence.end() ? SpecialKey::Undefined : it->second;
}

///
///
///


// TODO: don't memorize empty expressions
list<string> g_expressionHistory;
list<string>::iterator g_currentExpressionHistory = g_expressionHistory.end();
string g_currentExpression;
string::iterator g_currentPosition = g_currentExpression.begin();


void handleChar(char c)
{
  putchar(c);
  g_currentPosition = g_currentExpression.insert(g_currentPosition, c);
  ++g_currentPosition;

  int goBack = 0;
  for (auto it = g_currentPosition; it != g_currentExpression.end(); ++it, ++goBack)
  {
    putchar(*it);
  }
  for (int i = 0; i < goBack; ++i)
    printf("\x1B[\01D");
}

string getExpression()
{
  return g_expressionHistory.back();
}

void updateStringView()
{
  handleSpecialKey(SpecialKey::Home);
  bool endHistory = g_currentExpressionHistory == g_expressionHistory.end();

  size_t szNew = endHistory ? 0 : g_currentExpressionHistory->size();


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


  g_currentExpression = *g_currentExpressionHistory;
  // TODO: replace with puts && check
  for (auto c : g_currentExpression)
  {
    putchar(c);
  }
  g_currentPosition = g_currentExpression.end();
}

const int g_pgCount = 5;

void handleSpecialKey(const SpecialKey key) {
  switch (key) {
    case SpecialKey::Up:
      if (g_currentExpressionHistory != g_expressionHistory.begin()) {
        g_currentExpressionHistory = std::prev(g_currentExpressionHistory);
        updateStringView();
      }
      break;
    case SpecialKey::Down:
      if (g_currentExpressionHistory != g_expressionHistory.end()) {
        g_currentExpressionHistory = std::next(g_currentExpressionHistory);
        updateStringView();
      }
      break;
    case SpecialKey::PageUp:
      for (int i = 0; i < g_pgCount && g_currentExpressionHistory != g_expressionHistory.begin(); ++i)
      {
        handleSpecialKey(SpecialKey::Up);
      }
      break;
    case SpecialKey::PageDown:
      for (int i = 0; i < g_pgCount && g_currentExpressionHistory != g_expressionHistory.end(); ++i)
      {
        handleSpecialKey(SpecialKey::Down);
      }
      break;
    case SpecialKey::Left:
      if (g_currentPosition != g_currentExpression.begin())
      {
        --g_currentPosition;
        printf("\x1B[D");
      }
      break;
    case SpecialKey::Right:
      if (g_currentPosition != g_currentExpression.end())
      {
        ++g_currentPosition;
        printf("\x1B[C");
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
        printf("\b \b");
        g_currentPosition = g_currentExpression.erase(std::prev(g_currentPosition));

        int goBack = 0;
        for (auto it = g_currentPosition; it != g_currentExpression.end(); ++it, ++goBack)
        {
          putchar(*it);
        }
        putchar(' ');
        ++goBack;
        for (int i = 0; i < goBack; ++i)
          printf("\x1B[\01D");
      }
      break;
    case SpecialKey::EndLine:
      g_expressionHistory.push_back(move(g_currentExpression));
      g_currentExpressionHistory = g_expressionHistory.end();
      g_currentPosition = g_currentExpression.begin();
      
      putchar('\n');
      break;
    case SpecialKey::Undefined:
      // do nothing
      break;
  }
  return;
}

void initConsole()
{
  termios term;
  tcgetattr(0, &term); // 0 ~ stdin
  term.c_lflag &= ~ICANON;
  tcsetattr(0, TCSANOW, &term);
  setbuf(stdin, NULL);
}


void deinitConsole() {

}
