#pragma once

/**
 * @file IConnection.hpp
 * @brief Declaration of the Connection interface.
 */

#include <boost/asio.hpp>
#include <boost/asio/cancellation_signal.hpp>
#include <boost/json.hpp>
#include <boost/url.hpp>

namespace xapi
{
namespace internals
{

class IConnection
{
  public:
    virtual ~IConnection() = default;

    /**
     * @brief Asynchronously establishes secure WebSocket connection to the server.
     * @param url The URL to connect to.
     * @return An awaitable void.
     * @throw xapi::exception::ConnectionClosed if the connection fails.
     */
    virtual boost::asio::awaitable<void> connect(const boost::url &url) = 0;

    /**
     * @brief Asynchronously disconnects from the server.
     * @return An awaitable void.
     */
    virtual boost::asio::awaitable<void> disconnect() = 0;

    /**
     * @brief Makes an asynchronous request to the server.
     * @param command The command to send as a JSON value.
     * @return An awaitable void.
     * @throw xapi::exception::ConnectionClosed if the request fails.
     */
    virtual boost::asio::awaitable<void> makeRequest(const boost::json::object &command) = 0;

    /**
     * @brief Waits for a response from the server.
     * @return An awaitable boost::json::object with response from the server.
     * @throw xapi::exception::ConnectionClosed if the response fails.
     */
    virtual boost::asio::awaitable<boost::json::object> waitResponse() = 0;
};

} // namespace internals
} // namespace xapi
