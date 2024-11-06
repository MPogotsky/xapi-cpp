#pragma once

/**
 * @file Connection.hpp
 * @brief Defines the Connection class for managing connection.
 *
 * This file contains the definition of the Connection class, which is used for
 * establishing and managing connections, making requests, and handling responses.
 */

#include <boost/asio.hpp>
#include <boost/asio/cancellation_signal.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/json.hpp>
#include <boost/url.hpp>
#include <chrono>
#include <string>

namespace xapi
{
namespace internals
{

/**
 * @class Connection
 * @brief Manages connection and communication with a server using secure WebSocket.
 *
 * The Connection class encapsulates the functionality for establishing an SSL connection,
 * sending requests, and receiving responses.
 */
class Connection
{
  public:
    Connection() = delete;

    Connection(const Connection &other) = delete;
    Connection &operator=(const Connection &other) = delete;

    Connection(Connection &&other) noexcept;
    // Move assignment operator is not supported because of boost::beast::websocket::stream
    Connection &operator=(Connection &&other) = delete;

    /**
     * @brief Constructs a new Connection object.
     * @param ioContext The IO context for asynchronous operations.
     */
    explicit Connection(boost::asio::io_context &ioContext);

    virtual ~Connection();

    /**
     * @brief Asynchronously establishes secure WebSocket connection to the server.
     * @param url The URL to connect to.
     * @return An awaitable void.
     * @throw xapi::exception::ConnectionClosed if the connection fails.
     */
    boost::asio::awaitable<void> connect(const boost::url &url);

    /**
     * @brief Asynchronously disconnects from the server.
     * @return An awaitable void.
     */
    boost::asio::awaitable<void> disconnect();

    /**
     * @brief Makes an asynchronous request to the server.
     * @param command The command to send as a JSON value.
     * @return An awaitable void.
     * @throw xapi::exception::ConnectionClosed if the request fails.
     */
    boost::asio::awaitable<void> makeRequest(const boost::json::object &command);

    /**
     * @brief Waits for a response from the server.
     * @return An awaitable boost::json::object with response from the server.
     * @throw xapi::exception::ConnectionClosed if the response fails.
     */
    boost::asio::awaitable<boost::json::object> waitResponse();

  protected:
    // The IO context for asynchronous operations.
    boost::asio::io_context &m_ioContext;

  private:
    /**
     * @brief Establishes an SSL connection asynchronously.
     * @param results The resolved endpoints to attempt to connect to.
     * @param host The host name.
     * @return An awaitable void.
     * @throw xapi::exception::ConnectionClosed if the SSL connection fails.
     */
    boost::asio::awaitable<void> establishSSLConnection(const boost::asio::ip::tcp::resolver::results_type &results,
                                                        const char *host);

    /**
     * @brief Starts the keep-alive coroutine.
     * @param cancellationSlot The cancellation slot for stopping the coroutine.
     * @return An awaitable void.
     * @throw xapi::exception::ConnectionClosed if the ping fails.
     */
    boost::asio::awaitable<void> startKeepAlive(boost::asio::cancellation_slot cancellationSlot);

    /**
     * @brief Cancels all pending asynchronous operations and stops the keep-alive coroutine.
     * @return void.
     */
    void cancelAsyncOperations() noexcept;

    // SSL context, stores certificates.
    boost::asio::ssl::context m_sslContext;

    // The WebSocket stream.
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::beast::tcp_stream>> m_websocket;

    // Cancellation signal for stopping keepAlive coroutine
    boost::asio::cancellation_signal m_cancellationSignal;

    // Time of the last request.
    std::chrono::time_point<std::chrono::system_clock> m_lastRequestTime;

    // Timeout for requests.
    const std::chrono::milliseconds m_requestTimeout;

    // Default port for WebSocket connections.
    const std::string m_websocketDefaultPort;
};

} // namespace internals
} // namespace xapi
