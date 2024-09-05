#pragma once

/**
 * @file Exceptions.hpp
 * @brief Defines the exception classes for the XAPI.
 *
 * This file contains the definition of the exception classes used in the XAPI.
 */

#include <exception>
#include <string>

namespace xapi
{
namespace exception
{

/**
 * @class LoginFailed
 * @brief Exception class to indicate login failure.
 *
 * This exception is thrown when a login attempt fails.
 */
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

/**
 * @class ConnectionClosed
 * @brief Exception class to indicate that the connection has been closed.
 *
 * This exception is thrown when connection fails.
 */
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
