#include "OutputRepresentation.h"
#include "ohbTransform.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>

using namespace std;

namespace
{

const string g_outputName = "Output";

string printChars(const int64_t result)
{
  ostringstream buffer;
  auto stringResult = toChars(result);
  for (char character : stringResult)
  {
    if (isprint(character))
    {
      buffer << character;
      continue;
    }
    int value = static_cast<int>(static_cast<unsigned char>(character));

    buffer << '\\' << setfill('0') << setw(2) << hex << value;
  }

  return buffer.str();
}

} // namespace

OutputRepresentation::OutputRepresentation()
{
  m_outputs.push_back({'d', [](int64_t result) { return std::to_string(result); }});
  m_outputs.push_back({'h', [](int64_t result) { return toHex(result); }});
  m_outputs.push_back({'o', [](int64_t result) { return toOctal(result); }});
  m_outputs.push_back({'c', printChars});
  m_outputs.push_back({'b', [](int64_t result) { return toBinary(result); }});
}

void OutputRepresentation::printResult(int64_t result)
{
  for (auto it : m_outputs)
  {
    if (!it.isActive)
    {
      continue;
    }
    cout << it.character << ": " << it.function(result) << endl;
  }
}


bool OutputRepresentation::handleLetter(char letter, bool value)
{
  // going to handle only visible characters
  if (!isprint(letter) || isspace(letter))
  {
    return false;
  }
  auto it = std::find_if(m_outputs.begin(), m_outputs.end(),
                         [letter](const RepresentationInfo &info) { return info.character == letter; });
  if (it == m_outputs.end())
  {
    cerr << "No representation for letter '" << letter << "' found" << endl;
    return false;
  }
  it->isActive = value;
  return false;
}

static bool isOutputReprSettings(const string &expression)
{
  if (expression.size() < g_outputName.size())
  {
    return false;
  }
  for (size_t i = 0; i < g_outputName.size(); ++i)
  {
    if (tolower(g_outputName[i]) != tolower(expression[i]))
    {
      return false;
    }
  }

  return true;
}

bool OutputRepresentation::parseOutputSettings(const std::string &settings)
{

  if (!isOutputReprSettings(settings))
  {
    return false;
  }

  enum
  {
    Undefined,
    Set,
    Append,
    Subtract
  } settingType = Undefined;
  size_t startPos = g_outputName.size();

  if (settings[startPos] == '=')
  {
    settingType = Set;
    ++startPos;
  }
  else if (settings[startPos] == '+' && settings[startPos + 1] == '=')
  {
    settingType = Append;
    startPos += 2;
  }
  else if (settings[startPos] == '-' && settings[startPos + 1] == '=')
  {
    settingType = Subtract;
    startPos += 2;
  }
  else
  {
    return false;
  }
  assert(settingType != Undefined);

  if (settingType == Set)
  {
    for (auto &it : m_outputs)
    {
      it.isActive = false;
    }
    settingType = Append;
  }

  for (; startPos < settings.size(); ++startPos)
  {
    handleLetter(settings[startPos], settingType == Append);
  }

  return true;
}
