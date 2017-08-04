#pragma once

#include <deque>
#include <string>
#include <optional>

class ExpressionHistory {
public:
  ExpressionHistory();
  // extracts relative element
  const std::string *getElement(int relPos = 0);
  void push_back(std::string value);

private:
  std::deque<std::string> m_history;
  std::deque<std::string>::const_iterator m_it;
};

