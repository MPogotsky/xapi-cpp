#pragma once

#include <exception>
#include <iostream>

namespace xapi
{
namespace exception
{

class LoginFailed final : public std::exception
{
  public:
    LoginFailed(const std::string &message) : m_message(message)
    {
    }

    const char *what() const noexcept override
    {
        return m_message.c_str();
    }

  private:
    std::string m_message;
};

class ConnectionClosed final : public std::exception
{
  public:
    ConnectionClosed(const std::string &message) : m_message(message)
    {
    }

    const char *what() const noexcept override
    {
        return m_message.c_str();
    }

  private:
    std::string m_message;
};

} // namespace exception
} // namespace xapi
