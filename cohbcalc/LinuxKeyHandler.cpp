#include "ConsoleHandler.h"
#include <termios.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <utility>
#include <map>
#include <algorithm>

using namespace std;


int kbhit() {
  int bytesWaiting;
  ioctl(0, FIONREAD, &bytesWaiting);
  return bytesWaiting;
}

static termios oldT, newT;

void initConsole()
{
  tcgetattr(0, &oldT); /* save old terminal i/o settings */
  tcgetattr(0, &newT); // 0 ~ stdin
  newT.c_lflag &= ~ICANON;
  newT.c_lflag &= ~ECHO;
  tcsetattr(0, TCSANOW, &newT);
  setbuf(stdin, NULL);
}


void deinitConsole() {
  tcsetattr(0, TCSANOW, &oldT);
}


/* Read 1 character without echo */
char getch()
{
  return getchar();
}


#define CHKEY_LEFT                               27, 91, 68
#define CHKEY_RIGHT                              27, 91, 67
#define CHKEY_UP                                 27, 91, 65
#define CHKEY_DOWN                               27, 91, 66
#define CHKEY_CTRLLEFT               27, 91, 49, 59, 53, 68
#define CHKEY_CTRLRIGHT              27, 91, 49, 59, 53, 67
#define CHKEY_PAGEUP                        27, 91, 53, 126
#define CHKEY_PAGEDOWN                      27, 91, 54, 126
#define CHKEY_HOME                               27, 91, 72
#define CHKEY_END                                27, 91, 70
#define CHKEY_BACKSPACE                                 127
#define CHKEY_DELETE                        27, 91, 51, 126
#define CHKEY_ENDLINE                                    10

constexpr const int ch_left[] = { CHKEY_LEFT };
constexpr const int ch_right[] = { CHKEY_RIGHT };
constexpr const int ch_up[] = { CHKEY_UP };
constexpr const int ch_down[] = { CHKEY_DOWN };
constexpr const int ch_ctrlleft[] = { CHKEY_CTRLLEFT };
constexpr const int ch_ctrlright[] = { CHKEY_CTRLRIGHT };
constexpr const int ch_pgup[] = { CHKEY_PAGEUP };
constexpr const int ch_pgdown[] = { CHKEY_PAGEDOWN };
constexpr const int ch_home[] = { CHKEY_HOME };
constexpr const int ch_end[] = { CHKEY_END };
constexpr const int ch_backspace[] = { CHKEY_BACKSPACE };
constexpr const int ch_delete[] = { CHKEY_DELETE };
constexpr const int ch_endline[] = { CHKEY_ENDLINE };

#define PTRSIZE(constv) make_pair<const int*, int>(&*constv, sizeof(constv) / sizeof(constv[0]))
constexpr const pair<const int*, int> g_allSpecial[] = { PTRSIZE(ch_left) , PTRSIZE(ch_right), PTRSIZE(ch_up),
PTRSIZE(ch_down), PTRSIZE(ch_ctrlleft), PTRSIZE(ch_ctrlright),
PTRSIZE(ch_pgup), PTRSIZE(ch_pgdown), PTRSIZE(ch_home), PTRSIZE(ch_end),
PTRSIZE(ch_backspace), PTRSIZE(ch_delete), PTRSIZE(ch_endline) };


struct CharComparator {
  bool operator()(const pair<const int *, int> &left, const pair<const int *, int> &right)  const {
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
const map<pair<const int*, int>, SpecialKey, CharComparator> g_sequence = { MAPVALUE(ch_left, Left),
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
  const int sz = sizeof(g_allSpecial) / sizeof(g_allSpecial[0]);
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
  const int sz = sizeof(g_allSpecial) / sizeof(g_allSpecial[0]);
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
  return find(g_specialKeyStartSequence.begin(), g_specialKeyStartSequence.end(), c)
    != g_specialKeyStartSequence.end();
}

Character readChar()
{
  Character c;
  int read[g_maxSequenceLength];
  read[0] = getch();

  if (!isSpecialKeyStart(read[0])) {
    c = static_cast<char>(read[0]);
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
  while (kbhit()) {
    getch();
    notFullSequence = true;
  }
  if (notFullSequence)
  {
    return SpecialKey::Undefined;
  }

  auto it = g_sequence.find(make_pair(read, i));
  return it == g_sequence.end() ? SpecialKey::Undefined : it->second;
}

void backspace(int n)
{
  for (int i = 0; i < n; ++i)
    printf("\b \b");
}

void cursorLeft(int n)
{
  for (int i = 0; i < n; ++i)
    printf("\x1B[D");
}
void cursorRight(int n)
{
  for (int i = 0; i < n; ++i)
    printf("\x1B[C");
}
