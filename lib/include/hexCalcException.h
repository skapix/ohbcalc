#pragma once

#include <string>

class HCException : public std::exception
{
public:
  HCException(size_t position, const std::string &message)
    : pos(position)
    , message(message)
  {}
  HCException(const std::string &message)
    : pos(-1)
    , message(message)
  {}

  virtual const char*what() const noexcept override
  {
    return message.c_str();
  }

  size_t getPos() const noexcept
  {
    return pos;
  }

  void setPos(size_t newPos) noexcept
  {
    pos = newPos;
  }

private:
  size_t pos;
  std::string message;
};