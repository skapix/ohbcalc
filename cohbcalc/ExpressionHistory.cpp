#include "ExpressionHistory.h"

ExpressionHistory::ExpressionHistory()
  : m_it(m_history.cend())
{}


const std::string *ExpressionHistory::getElement(int relPos)
{
  if (m_history.empty())
  {
    return nullptr;
  }

  while (relPos < 0)
  {
    if (m_it == m_history.begin())
    {
      return &*m_it;
    }
    --m_it;
    ++relPos;
  }

  while (relPos > 0)
  {
    if (m_it == m_history.end())
    {
      return nullptr;
    }
    ++m_it;
    --relPos;
  }

  return m_it == m_history.cend() ? nullptr : &*m_it;
}

void ExpressionHistory::push_back(std::string value)
{
  m_history.push_back(std::move(value));
  m_it = m_history.end();
}
