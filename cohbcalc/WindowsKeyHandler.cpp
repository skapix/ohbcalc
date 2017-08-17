#include "ConsoleHandler.h"
#include <Windows.h>
#include <conio.h>
#include <cassert>
#include <map>
#include <iostream>

using namespace std;

static HANDLE g_inConsole = nullptr;
static HANDLE g_outConsole = nullptr;


void initConsole()
{
  g_inConsole = GetStdHandle(STD_INPUT_HANDLE);
  g_outConsole = GetStdHandle(STD_OUTPUT_HANDLE);
}

void deinitConsole()
{

}

static const map<int, SpecialKey> g_toSpecials =
{
  {VK_LEFT, SpecialKey::Left},
  {VK_RIGHT, SpecialKey::Right},
  {VK_UP, SpecialKey::Up},
  {VK_DOWN, SpecialKey::Down},
  {VK_PRIOR, SpecialKey::PageUp},
  {VK_NEXT, SpecialKey::PageDown},
  {VK_HOME, SpecialKey::Home},
  {VK_END, SpecialKey::End},
  {VK_DELETE, SpecialKey::Delete},
  {VK_BACK, SpecialKey::BackSpace},
  {VK_RETURN, SpecialKey::EndLine}
};

static const map<int, SpecialKey> g_toSpecialsCtrl =
{
  { VK_LEFT, SpecialKey::CtrlLeft },
  { VK_RIGHT, SpecialKey::CtrlRight }
};

Character readChar()
{
  INPUT_RECORD input;
  DWORD amount;
  while (true)
  {
    BOOL result = ReadConsoleInput(g_inConsole, &input, 1, &amount);
    assert(amount == 1 && result && "Failed to read console input");
    if (input.EventType != KEY_EVENT)
    {
      continue;
    }
    auto &keyEvent = input.Event.KeyEvent;
    if (!keyEvent.bKeyDown)
    {
      continue;
    }
    if (keyEvent.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED))
    {
      auto it = g_toSpecialsCtrl.find(keyEvent.wVirtualKeyCode);
      if (it != g_toSpecialsCtrl.end())
      {
        return it->second;
      }
      continue;
    }
    auto it = g_toSpecials.find(keyEvent.wVirtualKeyCode);
    if (it != g_toSpecials.end())
    {
      return it->second;
    }

    const char resultChar = input.Event.KeyEvent.uChar.AsciiChar;
    if (resultChar)
    {
      return resultChar;
    }
  }
  
}

static void moveCursor(int n)
{
  CONSOLE_SCREEN_BUFFER_INFO info;
  BOOL result = GetConsoleScreenBufferInfo(g_outConsole, &info);
  assert(result && "Can't move cursor");
  info.dwCursorPosition.X += static_cast<SHORT>(n);
  SetConsoleCursorPosition(g_outConsole, info.dwCursorPosition);
}

void cursorLeft(int n = 1)
{
  moveCursor(-n);
}
void cursorRight(int n = 1)
{
  moveCursor(n);
}
void backspace(int n = 1)
{
  cursorLeft(n);
  auto spaces = string(n, ' ');
  cout << spaces;
  cursorLeft(n);
}