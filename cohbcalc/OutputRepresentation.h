#pragma once
#include <cstdint>
#include <string>
#include <functional>

class OutputRepresentation
{
public:
  OutputRepresentation();
  void printResult(int64_t result);
  bool parseOutputSettings(const std::string &settings);

private:
  bool handleLetter(char letter, bool value);

private:
  struct RepresentationInfo
  {
    const char character;
    const std::function<std::string(int64_t)> function;
    bool isActive = true;
  };
  std::vector<RepresentationInfo> m_outputs;
};
