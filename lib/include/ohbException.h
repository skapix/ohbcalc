#pragma once

#include <string>

class OHBException : public std::exception
{
public:
  OHBException(size_t position, const std::string &message)
    : pos(position)
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